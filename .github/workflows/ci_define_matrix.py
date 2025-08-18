import os
import json

platforms = {
    "arterytekat32": "https://github.com/ArteryTek/platform-arterytekat32",
}

if __name__ == "__main__":
    matrix = []
    for keyboard in os.listdir("keyboards"):
        config_json_path = os.path.join("keyboards", keyboard, "config.json")

        if not os.path.exists(config_json_path):
            continue

        with open(config_json_path, "r") as f:
            config = json.load(f)

        packages = []
        if "platform" in config and config["platform"] in platforms:
            packages.append(platforms[config["platform"]])

        matrix.append({"keyboard": keyboard, "packages": " ".join(packages)})

    print(json.dumps(matrix))
