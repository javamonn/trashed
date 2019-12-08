module Permissions = Externals_Permissions;
module MediaStream = Externals_MediaStream;
module MediaDevices = Externals_MediaDevices;

[@bs.deriving accessors]
type status('data) =
  | Unknown
  | Unprompted
  | TransitionInProgress(Js.Promise.t(status('data)))
  | PermissionGranted(option('data))
  | PermissionRejected;

let permissionStatusChangeEvent = "trashcat-permissionStatusChange";

module Make =
       (
         Conf: {
           type data;
           let name: Permissions.permission;
           let onPrompt:
             unit =>
             Js.Promise.t((option(Permissions.Status.t), option(data)));
         },
       ) => {
  let status = ref(Unknown);
  let handleStatusChange = newStatus => {
    status := newStatus;

    let _ =
      Webapi.Dom.(
        window
        |> Window.dispatchEvent(Event.make(permissionStatusChangeEvent))
      );
    ();
  };

  let prompt = () => {
    let newStatus =
      Conf.onPrompt()
      |> Js.Promise.then_(((perm, data)) => {
           let nextStatus =
             switch (perm->Belt.Option.map(Permissions.Status.stateGet)) {
             | Some(`Granted) => PermissionGranted(data)
             | Some(`Denied) => PermissionRejected
             | _ => Unprompted
             };
           handleStatusChange(nextStatus);
           Js.Promise.resolve(nextStatus);
         })
      |> transitionInProgress;
    let _ = handleStatusChange(newStatus);
    newStatus;
  };

  let initialize = () => {
    Permissions.(query(Descriptor.make(~name=Conf.name)))
    |> Js.Promise.then_(s => {
         let initialStatus =
           switch (s->Belt.Option.map(Permissions.Status.stateGet)) {
           | Some(`Prompt) => Unprompted
           | Some(`Granted) => prompt()
           | Some(`Denied) => PermissionRejected
           | None => Unprompted
           };
         handleStatusChange(initialStatus);
         Js.Promise.resolve(initialStatus);
       })
    |> transitionInProgress
    |> handleStatusChange;
  };

  let use = () => {
    let (internalStatus, setStatus) = React.useState(() => status^);
    let _ =
      React.useEffect0(() => {
        let handlePermissionStatusChange = _ev => {
          setStatus(_ => status^);
        };
        Webapi.Dom.(
          window
          |> Window.addEventListener(
               permissionStatusChangeEvent,
               handlePermissionStatusChange,
             )
        );
        let _ = setStatus(_ => status^);
        Some(
          () =>
            Webapi.Dom.(
              window
              |> Window.removeEventListener(
                   permissionStatusChangeEvent,
                   handlePermissionStatusChange,
                 )
            ),
        );
      });
    let onSetStatus = newStatus => {
      handleStatusChange(newStatus);
    };

    (internalStatus, prompt, onSetStatus);
  };
};

module Camera =
  Make({
    let name = `Camera;
    type data = MediaStream.t;

    let onPrompt = () =>
      MediaDevices.MediaConstraints.(
        make(~audio=true, ~video=makeVideo(~facingMode="environment"), ())
      )
      |> MediaDevices.getUserMedia
      |> Js.Promise.then_(stream => {
           let perm =
             Permissions.Status.make(~state=`Granted)->Js.Option.some;
           Js.Promise.resolve((perm, Some(stream)));
         })
      |> Js.Promise.catch(_ex => {
           let perm = Permissions.Status.make(~state=`Denied)->Js.Option.some;
           Js.Promise.resolve((perm, None));
         });
  });

module Geolocation =
  Make({
    let name = `Geolocation;
    type data = Externals_Geolocation.position;

    let onPrompt = () =>
      Js.Promise.make((~resolve, ~reject) =>
        Externals_Geolocation.getCurrentPosition(
          position => {
            let perm =
              Permissions.Status.make(~state=`Granted)->Js.Option.some;
            let _ = resolve(. (perm, Some(position)));
            ();
          },
          _positionError => {
            let perm =
              Permissions.Status.make(~state=`Denied)->Js.Option.some;
            let _ = resolve(. (perm, None));
            ();
          },
          Externals_Geolocation.currentPositionOptions(
            ~enableHighAccuracy=true,
            (),
          ),
        )
      );
  });
