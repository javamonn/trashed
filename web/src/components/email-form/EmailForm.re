open Styles;

Utils.require("./email-form.module.css")

let component = ReasonReact.statelessComponent("EmailForm");

let make = _children => {
  let handleSubmit = ev => {
    let _ = ReactEvent.Form.preventDefault(ev);
    Js.log2("handleSubmit", ev);
  };
  {
    ...component,
    render: _self =>
      <form className={cn(["flex", "flex-row"])} onSubmit={handleSubmit}>
        <input
          type_="email"
          placeholder="Email Address"
          className={cn([
            "br1",
            "ba",
            "b--black",
            "pa_75",
            "bg-transparent",
            "mr2",
            "flex-auto"
          ])}
        />
        <Button
          type_="submit"
          label={ReasonReact.string("Submit")}
        />
      </form>
  };
}
