# This script generates a timestamp in the following format:
# Weekday, Month Day, Year HH:MM:SS
#
# Where:
# - Weekday is the abbreviated name of the day (e.g., Mon, Tue, Wed)
# - Month is the full name of the month (e.g., January, February, March)
# - Day is the day of the month (e.g., 01, 02, 03, ... 31)
# - Year is the full year (e.g., 2025)
# - HH:MM:SS is the time in 24-hour format (e.g., 13:45:00)
#
# Example output: "Tue, April 22, 2025 13:45:00"
# The timestamp is then copied to the system clipboard for easy access.

import time
import pyperclip  # Module for clipboard operations
from datetime import datetime

def get_current_timestamp():
    # Get the current time along with the abbreviated weekday in the format "Weekday, Month Day, Year HH:MM:SS"
    timestamp = datetime.now().strftime("%a, %B %d, %Y %H:%M:%S")

    # Debugging: Print timestamp type and value
    print(f"Timestamp: {timestamp} (Type: {type(timestamp)})")

    pyperclip.copy(timestamp)  # Copy the timestamp to clipboard
    print(f"Timestamp copied to clipboard: {timestamp}")

# Execute the function
get_current_timestamp()
