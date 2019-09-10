module MediaRecorder = VideoRecorder_MediaRecorder;
module FileInput = VideoRecorder_FileInput;

module CreateItemMutation = [%graphql
  {|
  mutation CreateItem($input: CreateItemInput!) {
    createItem(input: $input) {
      id
      file {
        bucket
        key
        region
      }
    }
  }
|}
];

[@react.component]
let make = () => {};
