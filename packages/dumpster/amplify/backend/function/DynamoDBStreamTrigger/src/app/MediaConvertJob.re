let make: (string, string, string) => Js.t('a) = [%raw
  {|
  (iamRole, sourceS3Path, destinationS3Path) => ({
    "Role": iamRole,
    "Settings": {
      "OutputGroups": [
        {
          "CustomName": "File Group",
          "Name": "File Group",
          "OutputGroupSettings": {
            "Type": "FILE_GROUP_SETTINGS",
            "FileGroupSettings": {
              "Destination": destinationS3Path,
            },
          },
          "Outputs": [
            {
              "NameModifier": "/clip-poster",
              "ContainerSettings": {
                "Container": "RAW",
              },
              "VideoDescription": {
                "CodecSettings": {
                  "Codec": "FRAME_CAPTURE",
                  "FrameCaptureSettings":
                    {"MaxCaptures": 1, "Quality": 50},
                },
              },
            },
            {
              "NameModifier": "/clip-thumbnail",
              "ContainerSettings": {
                "Container": "RAW",
              },
              "VideoDescription": {
                "CodecSettings": {
                  "Codec": "FRAME_CAPTURE",
                  "FrameCaptureSettings":
                    {"MaxCaptures": 1, "Quality": 25},
                },
                "Height": 320,
                "Width": 320,
              },
            },
            {
              "NameModifier": "/full-video",
              "ContainerSettings":
                {
                  "Container": "MP4",
                  "Mp4Settings": {
                    "CslgAtom": "INCLUDE",
                    "FreeSpaceBox": "EXCLUDE",
                    "MoovPlacement": "PROGRESSIVE_DOWNLOAD",
                  },
                },
              "VideoDescription": {
                "ScalingBehavior": "DEFAULT",
                "Sharpness": 50,
                "CodecSettings": {
                  "Codec": "H_264",
                  "H264Settings":
                    {
                      "InterlaceMode": "PROGRESSIVE",
                      "NumberReferenceFrames": 3,
                      "Syntax": "DEFAULT",
                      "Softness": 0,
                      "GopClosedCadence": 1,
                      "GopSize": 90,
                      "GopSizeUnits": "FRAMES",
                      "Slices": 1,
                      "GopBReference": "DISABLED",
                      "SlowPal": "DISABLED",
                      "SpatialAdaptiveQuantization": "ENABLED",
                      "TemporalAdaptiveQuantization": "ENABLED",
                      "FlickerAdaptiveQuantization": "DISABLED",
                      "EntropyEncoding": "CABAC",
                      "Bitrate": 5000000,
                      "FramerateControl": "SPECIFIED",
                      "RateControlMode": "CBR",
                      "CodecProfile": "MAIN",
                      "Telecine": "NONE",
                      "MinIInterval": 0,
                      "AdaptiveQuantization": "HIGH",
                      "CodecLevel": "AUTO",
                      "FieldEncoding": "PAFF",
                      "SceneChangeDetect": "ENABLED",
                      "QualityTuningLevel": "SINGLE_PASS",
                      "FramerateConversionAlgorithm": "DUPLICATE_DROP",
                      "UnregisteredSeiTimecode": "DISABLED",
                      "ParControl": "SPECIFIED",
                      "NumberBFramesBetweenReferenceFrames": 2,
                      "RepeatPps": "DISABLED",
                      "FramerateNumerator": 30,
                      "FramerateDenominator": 1,
                      "ParNumerator": 1,
                      "ParDenominator": 1,
                    }
                },
              },
            },
          ],
        },
      ],
      "AdAvailOffset": 0,
      "Inputs": [
        {
          "VideoSelector": {
            "ColorSpace": "FOLLOW",
          },
          "FilterEnable": "AUTO",
          "PsiControl": "USE_PSI",
          "FilterStrength": 0,
          "DeblockFilter": "DISABLED",
          "DenoiseFilter": "DISABLED",
          "TimecodeSource": "EMBEDDED",
          "FileInput": sourceS3Path,
        },
      ],
      "TimecodeConfig": {
        "Source": "EMBEDDED",
      },
    }
  })
|}
];

let make = (~iamRole, ~sourceS3Object, ~destinationS3Object) =>
  make(
    iamRole,
    sourceS3Object->Externals.S3Object.toString,
    destinationS3Object->Externals.S3Object.toString,
  );
