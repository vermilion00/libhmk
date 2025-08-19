# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.

import json
import os
import utils

Import("env")

PIP_PACKAGES = ["pip", "jsonschema"]

for package in PIP_PACKAGES:
    env.Execute(f"$PYTHONEXE -m pip install {package} --upgrade")

keyboard = env["PIOENV"]

# Load JSON files
kb_json = utils.get_kb_json(keyboard)
driver_json = utils.get_driver_json(keyboard)

# Validate JSON
import jsonschema

with open(os.path.join("scripts", "schema", "keyboard.schema.json")) as f:
    kb_schema = json.load(f)
with open(os.path.join("scripts", "schema", "driver.schema.json")) as f:
    driver_schema = json.load(f)

jsonschema.validate(kb_json, schema=kb_schema)
jsonschema.validate(driver_json, schema=driver_schema)
