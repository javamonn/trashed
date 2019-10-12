let make = (~iamRole, ~destinationS3Object, ~sourceS3Object) => {
  "Role": iamRole,
  "Settings": {
    "OutputGroups": [|
      {
        "CustomName": "File Group",
        "Name": "File Group",
        "OutputGroupSettings": {
          "Type": "FILE_GROUP_SETTINGS",
          "FileGroupSettings": {
            "Destination": destinationS3Object->Externals.S3Object.toString,
          },
        },
        "Outputs": [|
          {
            "NameModifier": "clip-poster",
            "ContainerSettings": Js.undefined,
            "VideoDescription": {
              "ScalingBehavior": Js.undefined,
              "Sharpness": Js.undefined,
              "CodecSettings": {
                "Codec": "FRAME_CAPTURE",
                "FrameCaptureSettings":
                  {"MaxCaptures": 1, "Quality": 50}->Js.Undefined.return,
                "H264Settings": Js.undefined,
              },
            },
            "Height": Js.undefined,
            "Width": Js.undefined,
          },
          {
            "NameModifier": "clip-thumbnail",
            "ContainerSettings": Js.undefined,
            "VideoDescription": {
              "ScalingBehavior": Js.undefined,
              "Sharpness": Js.undefined,
              "CodecSettings": {
                "Codec": "FRAME_CAPTURE",
                "FrameCaptureSettings":
                  {"MaxCaptures": 1, "Quality": 50}->Js.Undefined.return,
                "H264Settings": Js.undefined,
              },
            },
            "Height": 320->Js.Undefined.return,
            "Width": 320->Js.Undefined.return,
          },
          {
            "NameModifier": "full-video",
            "Height": Js.undefined,
            "Width": Js.undefined,
            "ContainerSettings":
              {
                "Container": "MP4",
                "Mp4Settings": {
                  "CslgAtom": "INCLUDE",
                  "FreeSpaceBox": "EXCLUDE",
                  "MoovPlacement": "PROGRESSIVE_DOWNLOAD",
                },
              }
              ->Js.Undefined.return,
            "VideoDescription": {
              "ScalingBehavior": "DEFAULT"->Js.Undefined.return,
              "Sharpness": 50->Js.Undefined.return,
              "CodecSettings": {
                "Codec": "H_264",
                "FrameCaptureSettings": Js.undefined,
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
                  ->Js.Undefined.return,
              },
            },
          },
        |],
      },
    |],
    "AdAvailOffset": 0,
    "Inputs": [|
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
        "FileInput": sourceS3Object->Externals.S3Object.toString,
      },
    |],
    "TimecodeConfig": {
      "Source": "EMBEDDED",
    },
  },
};
