#!/bin/bash

##
#  MakAir
#
#  Copyright: 2020, Makers For Life
#  License: Public Domain License
##

# Read arguments
while [ "$1" != "" ]; do
    argument_key=`echo $1 | awk -F= '{print $1}'`
    argument_value=`echo $1 | awk -F= '{print $2}'`

    case $argument_key in
        -v | --version)
            MAKAIR_CONTROL_VERSION="$argument_value"
            ;;
        *)
            echo "Unknown argument received: '$argument_key'"
            exit 1
            ;;
    esac

    shift
done

# Ensure release version is provided
if [ -z "$MAKAIR_CONTROL_VERSION" ]; then
  echo "No MakAir Control release version was provided, please provide it using '--version'"

  exit 1
fi

# Define release pipeline
function release_for_architecture {
    final_tar="v$MAKAIR_CONTROL_VERSION-$1.tar.gz"

    rm -rf ./makair-control/ && \
        RUSTFLAGS="-C link-arg=-s" cross build --target "$2" --release && \
        mkdir ./makair-control && \
        cp -p "target/$2/release/makair-control" ./makair-control/ && \
        tar -czvf "$final_tar" ./makair-control && \
        rm -r ./makair-control/
    release_result=$?

    if [ $release_result -eq 0 ]; then
        echo "Result: Packed architecture: $1 to file: $final_tar"
    fi

    return $release_result
}

# Run release tasks
ABSPATH=$(cd "$(dirname "$0")"; pwd)
BASE_DIR="$ABSPATH/../"

rc=0

pushd "$BASE_DIR" > /dev/null
    echo "Executing release steps for MakAir Control v$MAKAIR_CONTROL_VERSION..."

    # Move the telemetry library into current working directory
    # Notice: this feels a bit hacky, but for now 'cross' is not capable of passing \
    #   parent directories to its Docker runtime.
    mv ../telemetry ./
    sed -i '' 's/path = "..\/telemetry"/path = ".\/repo\/src\/software\/telemetry"/' ./Cargo.toml

    # Build releases
    release_for_architecture "armv7" "armv7-unknown-linux-musleabihf"
    rc=$?

    # Move back the telemetry library
    mv ./telemetry ../
    sed -i '' 's/path = ".\/repo\/src\/software\/telemetry"/path = "..\/telemetry"/' ./Cargo.toml

    if [ $rc -eq 0 ]; then
        echo "Success: Done executing release steps for MakAir Control v$MAKAIR_CONTROL_VERSION"
    else
        echo "Error: Failed executing release steps for MakAir Control v$MAKAIR_CONTROL_VERSION"
    fi
popd > /dev/null

exit $rc
