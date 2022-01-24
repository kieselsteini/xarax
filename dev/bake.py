import json


def write_maps(file):
    """ Convert the Tiled JSON export to a binary format """
    with open('./dev/world.json') as f:
        world = json.load(f)
    data = (bytearray(), bytearray())
    for group in world['layers']:
        for i in range(2):
            layer = group['layers'][i]
            tiles = [x - 1 if x > 0 else 0 for x in layer['data']]
            data[i].extend(bytes(tiles))
    file.write(data[0])
    file.write(data[1])


def write_strings(file):
    """ Convert text.txt to a binary format """
    data = bytearray((0,))
    info = []
    current = None
    with open('./dev/strings.txt', 'r') as f:
        for line in f:
            if line:
                if line[0] == '!':
                    parts = line[1:].split()
                    current = (int(parts[0]), int(parts[1]), int(parts[2]), len(data))
                    info.append(current)
                    text = []
                elif line[0] == '.':
                    data.extend(bytes('\n'.join(text), 'ascii'))
                    data.append(0) # C string terminator
                    current = None
                elif current:
                    text.append(line.rstrip())

    # write the the text-body + padding bytes
    file.write(data)
    file.write(bytes(1024 * 64 - len(data)))
    print('text_data', len(data))
    # write the info table + padding bytes
    for item in info:
        file.write(bytes((item[0], item[1], item[2], item[3] & 255, item[3] >> 8)))
    file.write(bytes(4096 * 5 - len(info) * 5))

if __name__ == '__main__':
    with open('world.dat', 'wb') as file:
        write_maps(file)
        write_strings(file)

