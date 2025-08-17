import argparse
import configparser
import os
import json

platforms = {
    "arterytekat32": "https://github.com/ArteryTek/platform-arterytekat32",
}

if __name__ == "__main__":
    matrix = []
    for keyboard in os.listdir("keyboards"):
        pio_ini_path = os.path.join("keyboards", keyboard, "pio.ini")

        if not os.path.exists(pio_ini_path):
            continue

        config = configparser.ConfigParser()
        config.read(pio_ini_path)

        packages = []
        for section in config.values():
            if "platform" in section and section["platform"] in platforms:
                packages.append(platforms[section["platform"]])

        matrix.append({"keyboard": keyboard, "packages": " ".join(packages)})

    print(json.dumps(matrix))
