import argparse
import struct
from pathlib import Path


def parse():
    parser = argparse.ArgumentParser( description="Create initramfs with provided files")
    parser.add_argument("files", nargs="+", help="Files to add")
    parser.add_argument("-o", "--output", required=True, help="Output initramfs file")
    args = parser.parse_args()

    files = [Path(f) for f in args.files]

    missing = [f for f in files if not f.is_file()]
    if missing:
        print("These files are missing or not regular files:")
        for f in missing:
            print(" -", f)
        return

    too_long = [f for f in files if len(f.name) > 255]
    if too_long:
        print("These files have too long names (max 255 chars):")
        for f in too_long:
            print(" -", f)
        return

    oversized = [f for f in files if f.stat().st_size > 2**32 - 1]
    if oversized:
        print("These files have are too big (max 4GiB):")
        for f in oversized:
            print(" -", f)
        return

    totalSize = 0
    for f in files:
        totalSize += f.stat().st_size
    if totalSize > 2**32 - 1:
        print(f"The total size of all files must be at most 4GiB ({ 2**32 - 1:,}) and is {totalSize:,}")

    entries = [
        (path, open(path, "rb"))
        for path in files
    ]
    outfile = open(args.output, "wb")

    return entries, outfile


def align2(x):
    return (x + 1) & ~1


def align16(x):
    return (x + 15) & ~15


def calcHeaderSize(entries):
    MAGIC_SIZE = 4
    VERSION_SIZE = 8
    FILE_COUNT_SIZE = 2
    out = MAGIC_SIZE + VERSION_SIZE + FILE_COUNT_SIZE
    for f, _ in entries:
        OFFSET_SIZE = 4
        SIZE_SIZE = 4
        NAME_LENGTH_SIZE = 1
        out += align2(OFFSET_SIZE + SIZE_SIZE + NAME_LENGTH_SIZE + len(f.name))
    return out


def main():
    parsed = parse()
    if parsed is None:
        return

    entries, outfile = parsed

    headerSize = calcHeaderSize(entries)
    offset = align16(headerSize)

    MAGIC = 0xB16B00B5
    VERSION = 0x00000000  # TODO:

    outfile.write(struct.pack(">IQH", MAGIC, VERSION, len(entries)))

    for path, file in entries:
        file_offset = offset
        file_size = path.stat().st_size
        file_name = path.name.encode()
        file_name_size = len(file_name)

        fmt = f">IIB{file_name_size}s"

        outfile.write(struct.pack(fmt, file_offset, file_size, file_name_size, file_name))

        if (file_name_size + 1) % 2 == 1:
            outfile.write(b"\x00")

        offset += align16(file_size)

    for i in range((16 - headerSize % 16) % 16):
        outfile.write(b"\x00")

    for path, file in entries:
        outfile.write(file.read())
        for i in range((16 - path.stat().st_size % 16) % 16):
            outfile.write(b"\x00")

        file.close()


if __name__ == "__main__":
    main()
