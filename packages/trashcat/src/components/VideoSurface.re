open Externals;
open Lib.Styles;
open Lib.Utils;

module MimeType = {
  [@bs.deriving jsConverter]
  type t = [
    | [@bs.as "video/webm"] `WEBM
    | [@bs.as "video/mp4"] `MP4
    | [@bs.as "video/quicktime"] `QUICKTIME
  ];

  let toJs = tToJs;
  let fromJs = tFromJs;

  let isSupported = mimeType => {
    switch (mimeType, Lib.Constants.browser->Bowser.getBrowserName) {
    | (_, None) => false
    | (`WEBM, Some(`Safari)) => false
    | (`WEBM, Some(_)) => true
    | (`MP4, Some(_)) => true
    | (`QUICKTIME, Some(`Safari)) => true
    | (`QUICKTIME, Some(_)) => false
    };
  };

  let sortPreference =
    fun
    | `WEBM => 1
    | `MP4 => 2
    | `QUICKTIME => 3;
};

[@bs.deriving accessors]
type src =
  | SrcObject(MediaStream.t)
  | SrcUrl(string)
  | SrcElement(array((string, MimeType.t)));

let maxDuration = 5.0;

[@react.component]
let make =
    (
      ~autoPlay=false,
      ~controls=false,
      ~forceLimitDuration=false,
      ~src,
      ~poster=?,
    ) => {
  let videoRef = React.useRef(Js.Nullable.null);

  let _ =
    React.useEffect0(() => {
      let _ =
        videoRef
        ->React.Ref.current
        ->Js.Nullable.toOption
        ->Belt.Option.map(
            doto(Webapi.Dom.Element.setAttribute("muted", "true")),
          )
        ->Belt.Option.map(
            doto(
              Webapi.Dom.Element.setAttribute(
                "disableRemotePlayback",
                "true",
              ),
            ),
          );
      None;
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          videoRef
          ->React.Ref.current
          ->Js.Nullable.toOption
          ->Belt.Option.map(elem => {
              let videoElem = VideoElement.unsafeAsVideoElement(elem);
              VideoElement.(
                switch (src) {
                | SrcObject(srcObject)
                    when
                      getSrcObject(videoElem)
                      !== Js.Undefined.return(srcObject) =>
                  let _ =
                    videoElem->setSrcObject(srcObject->Js.Undefined.return);
                  ();
                | _ => ()
                }
              );
            });
        None;
      },
      [|src|],
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          videoRef
          ->React.Ref.current
          ->Js.Nullable.toOption
          ->Belt.Option.map(elem => {
              let videoElem = elem->VideoElement.unsafeAsVideoElement;
              let paused = videoElem->VideoElement.paused;
              let readyState = videoElem->VideoElement.readyState;

              switch (readyState) {
              | _ when !autoPlay && !paused =>
                let _ = videoElem->VideoElement.pause;
                ();
              | `HaveNothing => ()
              | _ when autoPlay && paused =>
                videoElem
                ->VideoElement.setMuted(Js.Undefined.return(true))
                ->VideoElement.play
              | _ => ()
              };
            });
        None;
      },
      [|autoPlay|],
    );

  let handleLoadedMetadata = _ev => {
    let _ =
      videoRef
      ->React.Ref.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(elem =>
          if (autoPlay) {
            let _ =
              elem
              ->VideoElement.unsafeAsVideoElement
              ->VideoElement.setMuted(Js.Undefined.return(true))
              ->VideoElement.play;
            ();
          }
        );
    ();
  };

  let handleTimeUpdate = _ev => {
    let _ =
      if (forceLimitDuration) {
        let _ =
          videoRef
          ->React.Ref.current
          ->Js.Nullable.toOption
          ->Belt.Option.map(elem => {
              let videoElement = elem->VideoElement.unsafeAsVideoElement;
              if (VideoElement.getCurrentTime(videoElement) >= maxDuration) {
                let _ = VideoElement.setCurrentTime(videoElement, 0.);
                ();
              };
            });
        ();
      };
    ();
  };

  let handleError = _ev => {
    let _ =
      videoRef
      ->React.Ref.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(elem => {
          let videoError =
            elem->VideoElement.unsafeAsVideoElement->VideoElement.getError;
          let errorText =
            "VideoSurface error: "
            ++ videoError.message
            ++ ", code :"
            ++ string_of_int(videoError.code);
          let _ =
            try(errorText->Js.Exn.raiseError) {
            | Js.Exn.Error(err) => Sentry.captureException(err)
            };
          ();
        });
    ();
  };

  let children =
    switch (src) {
    | SrcElement(ss) =>
      ss
      ->Belt.Array.map(((src, type_)) =>
          <source type_={type_->MimeType.toJs} src key=src />
        )
      ->ReasonReact.array
    | _ => React.null
    };

  ReactDOMRe.createElementVariadic(
    "video",
    ~props=
      ReactDOMRe.objToDOMProps({
        "autoPlay": autoPlay,
        "controls": controls,
        "loop": !controls,
        "preload": "auto",
        "onError": handleError,
        "disableRemotePlayback": true,
        "playsInline": true,
        "onLoadedMetadata": autoPlay ? Some(handleLoadedMetadata) : None,
        "onTimeUpdate": forceLimitDuration ? Some(handleTimeUpdate) : None,
        "className":
          cn(["w-full", "h-full", "object-cover", "overflow-hidden"]),
        "ref": videoRef->ReactDOMRe.Ref.domRef,
        "muted": true,
        "poster": poster,
        "src":
          switch (src) {
          | SrcUrl(url) => Some(url)
          | _ => None
          },
      }),
    [|children|],
  );
};

module ContrastOverlay = {
  [@react.component]
  let make = (~height) =>
    <div
      className={cn(["absolute", "inset-x-0", "bottom-0"])}
      style={style(
        ~height=rem(height),
        ~background=
          "linear-gradient(0deg, rgba(0, 0, 0, 0.12) 80%, transparent)",
        (),
      )}
    />;
};
