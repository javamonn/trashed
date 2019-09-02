%raw
"import './app.pcss'";

[@react.component]
let make = () => {
  <VideoRecorder mimeType="video/webm;codecs=vp9" />;
};
