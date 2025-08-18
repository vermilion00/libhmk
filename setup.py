import argparse
import configparser
import json
import os

BUILD_FLAGS = """
-Wall
-Wextra
-Wno-unused-parameter"""

BUILD_SRC_FILTER = """
+<*>
-<.git/>
-<.svn/>
-<hardware/>"""

LIB_DEPS = """
https://github.com/hathach/tinyusb.git"""

UPLOAD_PROTOCOL = "dfu"

BOARD_BUILD_LINKER_SCRIPT = "linker/{ldscript}"

ENV_BUILD_FLAGS = """
${{env.build_flags}}
-Ihardware/{driver}/
-Ikeyboards/{keyboard}/
-Iinclude/"""

ENV_BUILD_SRC_FILTER = """
${{env.build_src_filter}}
+<hardware/{driver}/>"""


def validate_config_json(config_json):
    drivers = os.listdir("hardware")
    ldscripts = os.listdir("linker")

    required_keys = ["driver", "board", "ldscript", "framework", "platform"]
    for key in required_keys:
        if key not in config_json:
            raise ValueError(f"Missing required key: {key}")

    if config_json["driver"] not in drivers:
        raise ValueError(f"Invalid driver: {config_json['driver']}")
    if config_json["ldscript"] not in ldscripts:
        raise ValueError(f"Invalid ldscript: {config_json['ldscript']}")


if __name__ == "__main__":
    keyboards = os.listdir("keyboards")

    parser = argparse.ArgumentParser(description="PlatformIO Project Setup")
    parser.add_argument("--log", action="store_true", help="Enable logging")
    parser.add_argument(
        "--keyboard", "-k", choices=keyboards, required=True, help="Select a keyboard"
    )
    args = parser.parse_args()

    keyboard: str = args.keyboard

    config_json_path = os.path.join("keyboards", keyboard, "config.json")

    if not os.path.exists(config_json_path):
        raise FileNotFoundError(f"config.json not found for {keyboard}")
    with open(config_json_path, "r") as f:
        config_json = json.load(f)

    validate_config_json(config_json)

    # Global settings
    config = configparser.ConfigParser()
    config["env"] = {
        "build_flags": BUILD_FLAGS,
        "build_src_filter": BUILD_SRC_FILTER,
        "lib_deps": LIB_DEPS,
        "upload_protocol": UPLOAD_PROTOCOL,
    }

    # Environment-specific settings
    config[f"env:{keyboard}"] = {
        "board": config_json["board"],
        "board_build.ldscript": BOARD_BUILD_LINKER_SCRIPT.format(
            ldscript=config_json["ldscript"]
        ),
        "build_flags": ENV_BUILD_FLAGS.format(
            driver=config_json["driver"], keyboard=keyboard
        ),
        "build_src_filter": ENV_BUILD_SRC_FILTER.format(driver=config_json["driver"]),
        "framework": config_json["framework"],
        "platform": config_json["platform"],
    }

    if args.log:
        # Add print library
        config["env"]["lib_deps"] += "\nhttps://github.com/eyalroz/printf.git#develop"
        # Enable logging module
        config[f"env:{keyboard}"]["build_flags"] += "\n-DLOG_ENABLED"

    with open("platformio.ini", "w") as f:
        config.write(f)
