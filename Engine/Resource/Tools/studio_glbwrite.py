# Postpones evaluation of type annotations, allowing annotations to refer to names that are
# defined later in the module and keeping annotations from being evaluated at import time.
from __future__ import annotations

from array import array     # `array` stores compact, typed numeric values without requiring NumPy.
import json                 # JSON is used to serialize the glTF scene description embedded in the GLB file.
import math                 # Math helpers provide finite-number checks and infinity values for bounds calculations.
from pathlib import Path    # Path provides a platform-independent way to write the generated file.
import struct               # struct packs binary fields into the exact little-endian layout required by GLB.
import sys                  # sys is used to detect host byte order so binary numeric data is always written little-endian.

try:
    import numpy as np  # NumPy is an optional fast path for converting whole vertex/index arrays at once.
except ImportError:     
    np = None           # If NumPy is unavailable, the same functionality is implemented with Python loops and `array`.

_ARRAY_BUFFER = 34962           # glTF buffer-view target: vertex/attribute data intended for an array buffer.
_ELEMENT_ARRAY_BUFFER = 34963   # glTF buffer-view target: triangle indices intended for an element/index buffer.
_FLOAT = 5126                   # glTF component type code for 32-bit IEEE-754 floating-point numbers.
_UNSIGNED_SHORT = 5123          # glTF component type code for unsigned 16-bit integers.
_UNSIGNED_INT = 5125            # glTF component type code for unsigned 32-bit integers.
_TRIANGLES = 4                  # glTF primitive mode code meaning that vertices are interpreted as triangles.

# Pad a byte sequence to the next 4-byte boundary, as required for GLB chunks.
def _aligned(payload: bytes, fill: bytes = b"\x00") -> bytes:
    return payload + fill * (-len(payload) % 4) # The default padding byte is zero for binary data; callers can use spaces for JSON.

# Convert a value to float and reject NaN or infinity, which are invalid mesh coordinates.
def _finite(value, label: str) -> float: 
    number = float(value)
    if not math.isfinite(number):
        raise ValueError(f"{label} contains a non-finite component")
    return number

# Preserve the sign of the first zero when calculating a bound from float32 data.
def _first_extreme(column, value):
	# Python min/max retain the first equal value; NumPy's reductions can treat -0.0 and +0.0
	# differently, so this helper reproduces the intended first-occurrence behavior.
    # A running Python min/max keeps the first of equal values, so a zero bound
    # takes the sign of the first zero in the stream.
	# Only special-case zero, where the sign bit can matter even though -0.0 == +0.0.
    if value == 0.0:
        return float(column[int(np.argmax(column == 0.0))])	# Find the first zero in the column and return its original signed-zero representation.
    return float(value)										# Nonzero extrema can be returned directly as ordinary Python floats.

# Fast NumPy implementation for turning rows of vectors into packed float32 bytes.
def _vector_bytes_numpy(values, width: int, label: str, transform, include_bounds):
    rows = np.asarray(values, dtype=np.float64)										# Use float64 during validation/transformation to reduce intermediate precision loss.
    if rows.ndim != 2:																# This fast path only accepts a rectangular two-dimensional array.
        return None																	# Return None to tell the caller to use the more general Python implementation.
    if rows.shape[0] == 0:															# A mesh attribute must have at least one row; an empty array is not a useful stream.
        raise ValueError(f"{label} is empty")
    if rows.shape[1] < width:														# Every row must contain the requested number of components (3 for positions/normals, 2 for UVs).
        raise ValueError(f"{label} row 0 has fewer than {width} components")
    rows = rows[:, :width]															# Ignore extra components here; for example, UV blend weights are packed separately.
    if not np.isfinite(rows).all():													# Reject NaN and infinity before converting the values to binary floats.
        raise ValueError(f"{label} contains a non-finite component")				# Positions and normals use the game's coordinate convention, which differs from glTF's.
    if transform is _game_vector:													# Reorder axes and negate Y: (x, y, z) in game space becomes (x, z, -y).
        rows = np.stack((rows[:, 0], rows[:, 2], -rows[:, 1]), axis=1)				# Texture coordinates need their vertical axis flipped for the target glTF convention.
    elif transform is _gltf_uv:														# Keep U unchanged and replace V with 1 - V.
        rows = np.stack((rows[:, 0], 1.0 - rows[:, 1]), axis=1)						# Only the two transformations implemented above are supported by the NumPy fast path.
    elif transform is not None:														# Fall back rather than silently applying an unknown transform incorrectly.
        return None																	# Convert to float32 and make the memory contiguous so `tobytes()` emits a packed stream.
    packed = np.ascontiguousarray(rows.astype(np.float32))							# Initialize bounds to infinity so the first real value always replaces each bound.
    minimum = [math.inf] * width													# Negative infinity is the corresponding starting point for maximum calculations.
    maximum = [-math.inf] * width													# Bounds are only needed for attributes such as POSITION whose accessor advertises min/max.
    if include_bounds:																# NumPy calculates the per-component minimum across all vertices.
        low = packed.min(axis=0)													# NumPy calculates the per-component maximum across all vertices.
        high = packed.max(axis=0)
        minimum = [																	# Convert each minimum back to a Python float and preserve signed-zero behavior.
            _first_extreme(packed[:, index], low[index]) for index in range(width)	
        ]
        maximum = [																	# Do the same for maxima, maintaining deterministic handling of signed zero.
            _first_extreme(packed[:, index], high[index]) for index in range(width)
        ]
    if sys.byteorder != "little":													# GLB binary numeric data must be little-endian regardless of the computer running this code.
        packed = packed.byteswap()													# Byte-swap on big-endian hosts; little-endian hosts already have the required byte order.
    return packed.tobytes(), int(packed.shape[0]), minimum, maximum					# Return packed bytes, number of vectors, and per-component bounds.

# Shared entry point: use NumPy when possible, otherwise process vectors one at a time.
def _vector_bytes(values, width: int, label: str, transform=None, include_bounds=False):
    if np is not None:																	# The fallback handles generators and other row iterables that NumPy cannot interpret as a matrix.
        try:																			# Attempt the optimized implementation when the optional dependency is installed.
            result = _vector_bytes_numpy(
                values, width, label, transform, include_bounds
            )
        except (TypeError, ValueError) as error:										# Some unsupported input shapes/types should trigger the fallback rather than fail immediately.
            if isinstance(error, ValueError) and str(error).startswith(label):			# But errors explicitly identifying this attribute should be surfaced as genuine validation errors.
                raise																	# A `None` result means the fast path declined the input or transform.
            result = None																# If NumPy successfully handled the data, return its packed stream immediately.
        if result is not None:
            return result
    packed = array("f")																	# `array('f')` stores values as compact C-style floats for the non-NumPy path.
    minimum = [math.inf] * width														# Per-component minima start at positive infinity.
    maximum = [-math.inf] * width														# Per-component maxima start at negative infinity.
    count = 0																			# Count rows as they are consumed, including when `values` is a generator.
    for row in values:																	# Process each vertex/normal/UV row without requiring all input in memory at once.
        if len(row) < width:															# Reject short rows rather than silently inventing missing components.
            raise ValueError(f"{label} row {count} has fewer than {width} components")	# Identify the row number in the validation message.
        vector = tuple(_finite(row[index], label) for index in range(width))			# Convert each component to a finite float before storing it.
        if transform is not None:														# Apply the requested coordinate or UV transformation, if any.
            vector = transform(vector)													# Store transformed components, not the original input coordinate system.
        start = len(packed)																# Remember where this row begins so its stored float32 values can be read back for bounds.
        packed.extend(vector)															# Append all components to the packed float array.
        if include_bounds:																# Avoid calculating bounds for streams that do not need them.
            for index in range(width):													# Update the minimum and maximum for every component in this vector.
                value = float(packed[start + index])									# Read back the value after float32 conversion so bounds match the actual stored data.
                minimum[index] = min(minimum[index], value)								# Retain the smallest stored component value seen so far.
                maximum[index] = max(maximum[index], value)								# Retain the largest stored component value seen so far.
        count += 1																		# Increment the number of complete vectors processed.
    if count == 0:																		# Empty streams are invalid for mesh attributes.
        raise ValueError(f"{label} is empty")
    if sys.byteorder != "little":														# Match glTF's required little-endian byte order on big-endian machines.
        packed.byteswap()																# Swap each numeric element's bytes, rather than reversing the whole stream.
    return packed.tobytes(), count, minimum, maximum									# Return the packed stream and metadata needed to build a glTF accessor.

# Validate and pack triangle indices, choosing the smallest legal integer width.
def _index_bytes(values, vertex_count: int):
    if np is not None:															# NumPy can validate and convert all indices in bulk.
        indices = np.asarray(values, dtype=np.int64).ravel()					# Flatten nested index arrays into one sequence of scalar indices.
        if not indices.size or indices.size % 3:								# A triangle list must contain at least one index and a multiple of three indices.
            raise ValueError(
                "triangle indices must contain one or more complete triangles"
            )
        low, high = int(indices.min()), int(indices.max())						# Record the lowest and highest referenced vertex indices.
        if low < 0 or high >= vertex_count:										# Every index must refer to an existing vertex, and indices cannot be negative.
            raise ValueError(
                f"triangle index range {low}..{high} is outside {vertex_count} vertices"
            )
        if high <= 0xFFFF:														# 16 bits can represent indices from 0 through 65535, reducing file size when possible.
            packed = indices.astype("<u2")										# Explicitly request little-endian unsigned 16-bit storage.
            component_type = _UNSIGNED_SHORT									# Store the glTF enum value corresponding to unsigned short indices.
        else:
            packed = indices.astype("<u4")										# Use 32-bit indices when at least one referenced vertex index exceeds 65535.
            component_type = _UNSIGNED_INT										# Store the glTF enum value corresponding to unsigned integer indices.
        return packed.tobytes(), int(indices.size), component_type, low, high	# Return bytes, index count, component type, and bounds for the accessor.
    indices = tuple(int(value) for value in values)								# Fallback path: consume the values as Python integers when NumPy is unavailable.
    if not indices or len(indices) % 3:											# Apply the same nonempty, whole-triangle requirement as the NumPy path.
        raise ValueError("triangle indices must contain one or more complete triangles")
    low, high = min(indices), max(indices)										# Calculate index bounds for validation and glTF accessor metadata.
    if low < 0 or high >= vertex_count:											# Reject indices outside the vertex array.
        raise ValueError(
            f"triangle index range {low}..{high} is outside {vertex_count} vertices"
        )
    if high <= 0xFFFF:															# Choose 16-bit indices when every index fits; otherwise use 32-bit indices.
        packed = array("H", indices)											# `array('H')` stores unsigned short values.
        component_type = _UNSIGNED_SHORT										# Record the matching glTF component type.
    else:
        packed = array("I", indices)											# `array('I')` stores unsigned integer values for larger meshes.
        component_type = _UNSIGNED_INT											# Record the 32-bit index component type.
    if sys.byteorder != "little":												# Convert the host-native array representation to the GLB-required byte order.
        packed.byteswap()														# The conversion is unnecessary on little-endian machines.
    return packed.tobytes(), len(indices), component_type, low, high			# Return the same metadata as the optimized path.

# Convert a single 3D game-space vector into the target coordinate convention.
def _game_vector(vector):
    return vector[0], vector[2], -vector[1] # Axis remapping: preserve X, move Z into Y, and move Y into negative Z.

# Convert a single 2D texture coordinate into the target convention.
def _gltf_uv(vector):
    return vector[0], 1.0 - vector[1]   # Flip the vertical texture coordinate while leaving the horizontal coordinate unchanged.

# Build a complete binary glTF (GLB) file from Blender-exported mesh arrays.
# `*` makes all mesh attributes after `positions` keyword-only; `name` labels the mesh/node.
def encode_blender_mesh(positions, *, normals=None, uvs=None, indices=None, name="mesh") -> bytes:
    
    # Convert positions, validate them, and calculate the bounds required by glTF.
    position_bytes, vertex_count, position_min, position_max = _vector_bytes(positions, 3, "positions", _game_vector, include_bounds=True)  # Positions are 3-component vectors transformed from game coordinates.
    
    if indices is None and vertex_count % 3:                                                            # Without an index buffer, the vertex stream itself is drawn in groups of three vertices.
        raise ValueError(f"non-indexed render stream has {vertex_count} vertices, not whole triangles") # Reject a trailing one or two vertices that could not form a complete triangle.
        
    streams = []            # Keep track of the accessors that correspond to the streams added to the binary buffer.
    buffer_views = []       # Each bufferView describes where one packed attribute/index stream lives in the binary blob.
    accessors = []          # Each accessor describes how to interpret a bufferView as typed vectors or scalars.
    binary = bytearray()    # Accumulate all packed attribute/index bytes here; each stream is aligned to four bytes.

    # Helper that appends one attribute/index stream and registers its glTF metadata.
    def add_stream(payload, *, target, component_type, count, kind, minimum=None, maximum=None): # `target` identifies vertex data versus index data; `kind` is VEC3, VEC2, or SCALAR.

        offset = len(binary)                            # The stream begins at the current end of the binary payload.
        binary.extend(payload)                          # Append the stream's raw bytes to the shared GLB binary chunk.
        binary.extend(b"\x00" * (-len(binary) % 4))     # Align the next stream start to four bytes; padding is not part of this view's byteLength.
        view_index = len(buffer_views)                  # Give this stream the next bufferView index in the document.
        
        # Describe the stream's location within buffer 0.
        buffer_views.append(
            {
                "buffer": 0,                # All streams are stored in the single binary buffer declared below.
                "byteOffset": offset,       # Byte offset of this stream within that buffer.
                "byteLength": len(payload), # Number of meaningful bytes in the stream, excluding alignment padding.
                "target": target,           # Tell glTF which kind of GPU buffer this data is intended for.
            }
        )

        # An accessor tells glTF how many elements to read and how to decode them.
        accessor = {
            "bufferView": view_index,           # Link the accessor to the bufferView created just above.
            "byteOffset": 0,                    # Data begins at the start of its bufferView.
            "componentType": component_type,    # Numeric encoding, such as float32 or unsigned 16-bit integer.
            "count": count,                     # Number of vectors/scalars represented by this accessor.
            "type": kind,                       # Element shape: VEC3 = three components, VEC2 = two, SCALAR = one.
        }

        if minimum is not None:				# Position bounds and index bounds are optional glTF accessor metadata.
            accessor["min"] = minimum		# Store minimum values when supplied.
        if maximum is not None:				# Store maximum values when supplied.
            accessor["max"] = maximum
        accessors.append(accessor)			# Register the completed accessor in document order.
        streams.append(len(accessors) - 1)	# Keep a parallel list of accessor indices; return the index of this new accessor.
        return streams[-1]

	# Add the required position stream and advertise its bounds.
    position_accessor = add_stream(
        position_bytes,
        target=_ARRAY_BUFFER,
        component_type=_FLOAT,
        count=vertex_count,
        kind="VEC3",
        minimum=position_min,
        maximum=position_max,
    )
	# glTF names vertex-position data with the standard POSITION attribute.
    attributes = {"POSITION": position_accessor}

	# Normals are optional, but if supplied they must have one vector per position.
    if normals is not None:
	# Normals use the same coordinate-axis conversion as positions.
        normal_bytes, normal_count, _minimum, _maximum = _vector_bytes(
            normals, 3, "normals", _game_vector
        )
	# Vertex attributes are matched by row index, so counts must agree.
        if normal_count != vertex_count:
            raise ValueError(
                f"normal count {normal_count} does not match {vertex_count} positions"
            )
	# Add normals as float32 VEC3 vertex data and associate them with the mesh.
        attributes["NORMAL"] = add_stream(
            normal_bytes,
            target=_ARRAY_BUFFER,
            component_type=_FLOAT,
            count=normal_count,
            kind="VEC3",
        )

	# UV texture coordinates are optional and must align one-to-one with vertices.
    if uvs is not None:
	# Store the first two UV components after flipping the vertical coordinate.
        uv_bytes, uv_count, _minimum, _maximum = _vector_bytes(uvs, 2, "UVs", _gltf_uv)
	# Ensure every position has a corresponding UV pair.
        if uv_count != vertex_count:
            raise ValueError(
                f"UV count {uv_count} does not match {vertex_count} positions"
            )
	# TEXCOORD_0 is glTF's standard name for the first texture-coordinate set.
        attributes["TEXCOORD_0"] = add_stream(
            uv_bytes,
            target=_ARRAY_BUFFER,
            component_type=_FLOAT,
            count=uv_count,
            kind="VEC2",
        )
	# A third UV component, when present, is treated as ReSkate-specific blend weight data.
        if len(uvs) and len(uvs[0]) > 2:
	# NumPy can extract the third column efficiently; otherwise slice it from each row.
            weights = (
                np.asarray(uvs)[:, 2:3] if np is not None else [row[2:3] for row in uvs]
            )
	# Pack one scalar blend weight per vertex without applying a coordinate transform.
            weight_bytes, count, _minimum, _maximum = _vector_bytes(
                weights, 1, "blend weights"
            )
	# _RESKATE_BLEND is a custom attribute consumed by ReSkate-specific tooling/runtime code.
            attributes["_RESKATE_BLEND"] = add_stream(
                weight_bytes,
                target=_ARRAY_BUFFER,
                component_type=_FLOAT,
                count=count,
                kind="SCALAR",
            )

	# Create one triangle primitive containing the attributes assembled above.
    primitive = {"attributes": attributes, "mode": _TRIANGLES}
	# An explicit index buffer lets triangles reuse vertices; omit it for sequential vertices.
    if indices is not None:
	# Validate indices and choose 16- or 32-bit storage based on the largest index.
        index_bytes, index_count, component_type, low, high = _index_bytes(
            indices, vertex_count
        )
	# Add the index stream and attach its accessor index to the primitive.
        primitive["indices"] = add_stream(
            index_bytes,
            target=_ELEMENT_ARRAY_BUFFER,
            component_type=component_type,
            count=index_count,
            kind="SCALAR",
            minimum=[low],
            maximum=[high],
        )
	# Assemble the glTF JSON document that describes the mesh and its binary data.
    document = {
	# Declare glTF 2.0 and identify this module as the file generator.
        "asset": {"version": "2.0", "generator": "ReSkate normalized GLB writer"},
	# Select scene 0 as the file's default scene.
        "scene": 0,
	# The default scene contains the single mesh node created below.
        "scenes": [{"nodes": [0]}],
	# A node places the mesh into the scene and gives it a readable name.
        "nodes": [{"mesh": 0, "name": str(name)}],
	# The mesh contains the one triangle primitive and its vertex attributes.
        "meshes": [{"name": str(name), "primitives": [primitive]}],
	# Declare the size of the single binary buffer, excluding GLB chunk headers/padding.
        "buffers": [{"byteLength": len(binary)}],
	# Map each logical stream to its location in the binary buffer.
	    "bufferViews": buffer_views,
	# Describe how each stream's bytes should be interpreted.
        "accessors": accessors,
    }
	# JSON chunks must be padded to a four-byte boundary; glTF permits spaces as padding.
    json_chunk = _aligned(
	# Use deterministic compact JSON: no unnecessary whitespace and stable key ordering.
        json.dumps(
	# ASCII escaping keeps the JSON encoding predictable; separators remove formatting spaces.
            document, ensure_ascii=True, separators=(",", ":"), sort_keys=True
        ).encode("utf-8"),
        b" ",
    )
	# Freeze the accumulated binary bytearray into immutable bytes for the final file.
    binary_chunk = bytes(binary)
	# GLB total size includes the 12-byte header and both 8-byte chunk headers.
    total = 12 + 8 + len(json_chunk) + 8 + len(binary_chunk)
	# Emit the GLB header, JSON chunk, and BIN chunk in the required order.
    return (
	# Four-byte magic signature identifying the file as binary glTF.
        b"glTF"
	# Header fields: version 2 and total file length, both little-endian uint32 values.
        + struct.pack("<II", 2, total)
	# JSON chunk header: padded chunk length and JSON chunk type identifier.
        + struct.pack("<II", len(json_chunk), 0x4E4F534A)
	# Append the padded UTF-8 JSON document.
        + json_chunk
	# BIN chunk header: binary payload length and BIN chunk type identifier.
        + struct.pack("<II", len(binary_chunk), 0x004E4942)
	# Append all packed mesh streams and their inter-stream alignment padding.
        + binary_chunk
    )

# Convenience wrapper that encodes a mesh and writes the resulting GLB to disk.
# Accepts the same attributes as the encoder plus the destination path.
# This function writes the file and intentionally returns no value.
def write_blender_mesh(path, positions, *, normals=None, uvs=None, indices=None, name="mesh") -> None:
    payload = encode_blender_mesh(positions, normals=normals, uvs=uvs, indices=indices, name=name)  # Perform all validation and binary/document construction in the shared encoder.
    Path(path).write_bytes(payload)                                                                 # Write the complete GLB payload to the requested path, replacing its contents if it exists.

