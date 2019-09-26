open Externals;
open Lib;

let handle = r => {
  let job =
    MediaConvertJob.make(
      ~iamRole=Constants.awsMediaConvertIamRole,
      ~sourceS3Object=s3Object,
      ~destinationS3Object=
        S3Object.make(
          ~bucket="trashcat",
          ~key="public/item-video" ++ s3Object->S3Object.namePathPartGet,
        ),
    );
  
  /** FIXME: Should probably keep track of MediaConvert job state */
  AWSSDK.MediaConvert.(service->createJob(job)->promise)
    |> Js.Promise.then_((_job) => Js.Promise.resolve());
};
