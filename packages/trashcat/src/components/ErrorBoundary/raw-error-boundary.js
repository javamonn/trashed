import * as React from "react";

class RawErrorBoundary extends React.Component {
  constructor (props) {
    super(props)
    this.state = { isErrored: false }
  }

  static getDerivedStateFromError() {
    return { isErrored: true };
  }

  componentDidCatch(error) {
    this.props.onComponentDidCatch(error);
  }

  render() {
    if (this.state.isErrored) {
      return this.props.renderErrorBoundary();
    } else {
      return this.props.children;
    }
  }
}

export default RawErrorBoundary;
