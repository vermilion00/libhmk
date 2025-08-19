import argparse
import configparser
import json
import os


def validate_config(config):
    required_keys = ["driver", "board", "ldscript", "framework", "platform"]
    for key in required_keys:
        if key not in config:
            raise ValueError(f"Missing required key: {key}")

    if config["driver"] not in os.listdir("hardware"):
        raise ValueError(f"Invalid driver: {config['driver']}")
    if config["ldscript"] not in os.listdir("linker"):
        raise ValueError(f"Invalid ldscript: {config['ldscript']}")


if __name__ == "__main__":
    keyboards = os.listdir("keyboards")

    parser = argparse.ArgumentParser(description="PlatformIO Project Setup")
    parser.add_argument("--log", action="store_true", help="Enable logging")
    parser.add_argument(
        "--keyboard", "-k", choices=keyboards, required=True, help="Select a keyboard"
    )
    args = parser.parse_args()

    keyboard: str = args.keyboard

    config_path = os.path.join("keyboards", keyboard, "config.json")
    if not os.path.exists(config_path):
        raise FileNotFoundError(f"config.json not found for {keyboard}")
    with open(config_path, "r") as f:
        config = json.load(f)

    validate_config(config)

    build_flags = [
        "${env.build_flags}",
        # We prioritize including driver and keyboard headers.
        f"-Ihardware/{config['driver']}/",
        f"-Ikeyboards/{keyboard}/",
        "-Iinclude/",
    ]
    build_src_filter = [
        "${env.build_src_filter}",
        "-<hardware/>",
        f"+<hardware/{config['driver']}/>",
    ]
    build_src_flags = [
        "${env.build_src_flags}",
        "-Werror",
        "-Wall",
        "-Wextra",
        "-Wsign-conversion",
        "-Wswitch-default",
        "-Wswitch",
        "-Wdouble-promotion",
        "-Wstrict-prototypes",
        "-Wno-unused-parameter",
    ]
    extra_scripts = ["pre:tools/metadata.py"]
    lib_deps = ["https://github.com/hathach/tinyusb.git"]

    if args.log:
        # Enable logging module
        lib_deps.append("https://github.com/eyalroz/printf.git#develop")
        build_src_flags.append("-DLOG_ENABLED")

    pio_config = configparser.ConfigParser()
    pio_config[f"env:{keyboard}"] = {
        "board": config["board"],
        "board_build.ldscript": f"linker/{config['ldscript']}",
        "build_flags": "\n".join(build_flags),
        "build_src_filter": "\n".join(build_src_filter),
        "build_src_flags": "\n".join(build_src_flags),
        "extra_scripts": "\n".join(extra_scripts),
        "framework": config["framework"],
        "lib_deps": "\n".join(lib_deps),
        "platform": config["platform"],
        "upload_protocol": "dfu",
    }

    with open("platformio.ini", "w") as f:
        pio_config.write(f)
