#!/bin/bash

# To add a new software, add the software slug name and the corresponding mode in following arrays
softwareNames=("integration-test" "production" "qualification")
softwareModes=("MODE_INTEGRATION_TEST" "MODE_PROD" "MODE_QUALIFICATION")

# To add a new hardware version, add the hardware slug name and the corresponding version in the following arrays
hardwareVersionNames=("HW1" "HW2")
hardwareVersions=("1" "2")

for softwareIndex in "${!softwareNames[@]}"
do
    for hardwareIndex in "${!hardwareVersionNames[@]}"
    do
      export SOFTWARE_FULLNAME="respirator-${softwareNames[$softwareIndex]}-${hardwareVersionNames[$hardwareIndex]}-dev"
      export MODE=${softwareModes[$softwareIndex]}
      export HARDWARE_VERSION=${hardwareVersions[$hardwareIndex]}

      arduino-cli cache clean
      build-wrapper-linux-x86-64 --out-dir sonarcloud/${SOFTWARE_FULLNAME}/bo bash sonarcloud/build.sh > /dev/null
      sonar-scanner -Dsonar.cfamily.cache.path=$HOME/.sonar/cache/${SOFTWARE_FULLNAME}          \
        -Dsonar.projectKey=${SOFTWARE_FULLNAME}                                                 \
        -Dsonar.projectVersion=dev                                                              \
        -Dsonar.sources=.                                                                       \
        -Dsonar.cfamily.build-wrapper-output=sonarcloud/${SOFTWARE_FULLNAME}/bo                 \
        -Dsonar.cfamily.cache.enabled=true                                                      \
        -Dsonar.cfamily.threads=8                                                               \
        -Dsonar.scm.exclusions.disabled=true

    done
done