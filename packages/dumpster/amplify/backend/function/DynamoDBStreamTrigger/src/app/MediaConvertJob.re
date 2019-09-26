open Externals;

let make = (~iamRole, ~destinationS3Object, ~sourceS3Object) => {
  "role": iamRole,
  "settings": {
    "outputGroups": {
      "customName": "File Group",
      "name": "File Group",
      "outputGroupSettings": {
        "type": "FILE_GROUP_SETTINGS",
        "fileGroupSettings": {
          "destination": destinationS3Object->Externals.S3Object.toString,
        },
      },
      "outputs": [|
        {
          "containerSettings": {
            "container": "MP4",
            "mp4Settings": {
              "clsgAtom": "INCLUDE",
              "freeSpaceBox": "EXCLUDE",
              "moovPlacement": "PROGRESSIVE_DOWNLOAD",
            },
          },
          "videoDescription": {
            "scalingBehavior": "DEFAULT",
            "sharpness": 50,
            "codecSettings": {
              "codec": "H_264",
              "h264Settings": {
                "interlaceMode": "PROGRESSIVE",
                "numberReferenceFrames": 3,
                "syntax": "DEFAULT",
                "softness": 0,
                "gopClosedCadence": 1,
                "gopSize": 90,
                "gopSizeUnits": "FRAMES",
                "slices": 1,
                "gopBReference": "DISABLED",
                "slowPal": "DISABLED",
                "spatialAdaptiveQuantization": "ENABLED",
                "temporalAdaptiveQuantization": "ENABLED",
                "flickerAdaptiveQuantization": "DISABLED",
                "entropyEncoding": "CABAC",
                "bitrate": 5000000,
                "framerateControl": "SPECIFIED",
                "rateControlMode": "CBR",
                "codecProfile": "MAIN",
                "telecine": "NONE",
                "minIInterval": 0,
                "adaptiveQuantization": "HIGH",
                "codecLevel": "AUTO",
                "fieldEncoding": "PAFF",
                "sceneChangeDetect": "ENABLED",
                "qualityTuningLevel": "SINGLE_PASS",
                "framerateConversionAlgorithm": "DUPLICATE_DROP",
                "unregisteredSeiTimecode": "DISABLED",
                "parControl": "SPECIFIED",
                "numberBFramesBetweenReferenceFrames": 2,
                "repeatPps": "DISABLED",
                "framerateNumerator": 30,
                "framerateDenominator": 1,
                "parNumerator": 1,
                "parDenominator": 1,
              },
            },
          },
          "audioDescriptions": [|
            {
              "audioTypeControl": "FOLLOW_INPUT",
              "audioSourceName": "Audio Selector 1",
              "languageCodeControl": "FOLLOW_INPUT",
              "codecSettings": {
                "codec": "AAC",
                "aacSettings": {
                  "audioDescriptionBroadcasterMix": "NORMAL",
                  "rateControlMode": "CBR",
                  "codecProfile": "LC",
                  "codingMode": "CODING_MODE_2_0",
                  "rawFormat": "NONE",
                  "sampleRate": 48000,
                  "specification": "MPEG4",
                  "bitrate": 64000,
                },
              },
            },
          |],
        },
      |],
    },
    "adAvailOffset": 0,
    "inputs": [|
      {
        "audioSelectors": {
          "Audio Selector 1": {
            "offset": 0,
            "defaultSelection": "NOT_DEFAULT",
            "programSelection": 1,
            "selectorType": "TRACK",
            "tracks": [|1|],
          },
        },
        "videoSelector": {
          "colorSpace": "FOLLOW",
        },
        "filterEnable": "AUTO",
        "psiControl": "USE_PSI",
        "filterStrength": 0,
        "deblockFilter": "DISABLED",
        "denoiseFilter": "DISABLED",
        "timecodeSource": "EMBEDDED",
        "fileInput": sourceS3Object->Externals.S3Object.toString,
      },
    |],
    "timecodeConfig": {
      "source": "EMBEDDED",
    },
  },
};
