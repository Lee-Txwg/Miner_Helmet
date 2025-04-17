import time
import pyperclip  # Module for clipboard operations
from datetime import datetime

def get_current_timestamp():
    # Get the current time in the format "Month Day, Year HH:MM:SS"
    timestamp = datetime.now().strftime("%B %d, %Y %H:%M:%S")

    # Debugging: Print timestamp type and value
    print(f"Timestamp: {timestamp} (Type: {type(timestamp)})")

    pyperclip.copy(timestamp)  # Copy the timestamp to clipboard
    print(f"Timestamp copied to clipboard: {timestamp}")

# Execute the function
get_current_timestamp()
