import uuid
import struct

def uuid_to_windows_bytes(guid: uuid.UUID) -> bytes:
    fields = guid.fields
    
    return struct.pack(
        '<IHH',
        fields[0],
        fields[1],
        fields[2],
    ) + struct.pack(
        '>H',
        (fields[3] << 8) | fields[4]
    ) + guid.node.to_bytes(6, 'big')

def create_config_file(guid_str: str, filepath: str, output_file: str):
    try:
        guid = uuid.UUID(guid_str)
        guid_bytes = uuid_to_windows_bytes(guid)
    except ValueError as e:
        raise ValueError(f"Invalid GUID: {e}") 

    filepath_bytes = filepath.encode('utf-16le')

    with open(output_file, 'wb') as f:
        f.write(guid_bytes)
        f.write(filepath_bytes)

if __name__ == "__main__":
    disk_guid = "9F260710-C337-487C-86FA-4F5D19C9BEB5"
    file_path = r"\boot\conf"
    output_path = "conf.meta"

    create_config_file(disk_guid, file_path, output_path)
