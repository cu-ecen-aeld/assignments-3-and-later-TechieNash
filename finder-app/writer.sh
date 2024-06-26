#!/bin/sh

# Check if both arguments are provided
if [ $# -ne 2 ]; then
  echo "Error: Both a file path and a text string must be provided."
  exit 1
fi

writefile=$1
writestr=$2

# Create the directory path if it doesn't exist
mkdir -p "$(dirname "$writefile")"

# Write the text string to the file, overwriting it if it exists
echo "$writestr" > "$writefile"

# Check if the file was created successfully
if [ $? -ne 0 ]; then
  echo "Error: The file could not be created."
  exit 1
fi

echo "File created successfully with the specified content."
exit 0
