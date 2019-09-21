open Lib;
open Externals;

let handle = s3Object => {
  let job =
    MediaConvertJob.make(
      ~iamRole=Constants.awsMediaConvertIamRole,
      ~sourceS3Object=s3Object,
      ~destinationS3Object=
        S3Object.make(
          ~bucket="trashcat",
          ~key="public/item-video" ++ s3Object->pathGet,
        ),
    );
  ();
};
