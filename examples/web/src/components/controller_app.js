import NavControl from './NavControl.js';
import VideoPlayer from './VideoPlayer.js';
import SelectInput from './SelectInput.js';
import ElementsControl from './ElementsControl.js';
import ChangeProperty from './ChangeProperty.js';
import BusConsole from './BusConsole.js'
import BusControl from './BusControl.js'
import FooterControl from './FooterControl.js'


var gstcG = new GstdClient();
if (document.getElementById("controller")) {
    Vue.prototype.$datas = {
        file: '',
        videoElement: null,
        paused: false,
        gstc: gstcG,
        componentKey: 0,
        selected_element: null,
        elements: [],
        selected_properties: null,
        properties: [],
        speed: 1,
        direction: 1,
        checked: true,
        selected_input: "File",
        bus_enable: true,
        timeout: 10000000
    }

}
export default {
    name: 'controller_app',
    components: {
        NavControl,
        VideoPlayer,
        SelectInput,
        ElementsControl,
        ChangeProperty,
        BusConsole,
        BusControl,
        FooterControl

    },
    data() {
        return {
            gstc: this.$datas.gstc,
            checked: this.$datas.checked,
            selected_input: this.$datas.selected_input,
            bus_enable: this.$datas.bus_enable,
            appConfig: true
        }
    },
    template: `
    <div class="container mx-auto p-4">
    <nav-control name="GSTD HTTP controller" ></nav-control>
      <video-player :config="appConfig" :name="gstc" :enable="checked"></video-player>
      <elements-control :name="gstc"></elements-control>
      <change-property :name="gstc"></change-property>
      <bus-console :config="appConfig" :enable="bus_enable"></bus-console>
      <bus-control ></bus-control>
      <footer-control></footer-control>
    </div>
  `,
};