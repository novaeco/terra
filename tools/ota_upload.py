#!/usr/bin/env python3
"""
Upload a firmware binary to an ESP32 device using the OTA HTTP API.

This script is a placeholder that demonstrates how you might send
the contents of a firmware file via HTTP POST to the `/api/v1/ota/update`
endpoint on the device.  The real implementation would handle
authentication, error checking and response parsing.
"""

import argparse
import requests


def main():
    parser = argparse.ArgumentParser(description="Upload firmware via OTA API")
    parser.add_argument("device_url", help="Base URL of the device, e.g. http://192.168.1.100")
    parser.add_argument("firmware", help="Path to firmware binary")
    parser.add_argument("token", help="Bearer token for authentication")
    args = parser.parse_args()

    with open(args.firmware, "rb") as f:
        files = {"firmware": ("firmware.bin", f, "application/octet-stream")}
        headers = {"Authorization": f"Bearer {args.token}"}
        url = args.device_url.rstrip("/") + "/api/v1/ota/update"
        print(f"Uploading {args.firmware} to {url}...")
        resp = requests.post(url, files=files, headers=headers)
        print("Status:", resp.status_code)
        print("Response:", resp.text)


if __name__ == "__main__":
    main()