import NavControl from './NavControl.js';
import VideoPlayer from './VideoPlayer.js';
import SelectInput from './SelectInput.js';
import ElementsControl from './ElementsControl.js';
import ChangeProperty from './ChangeProperty.js';
import BusConsole from './BusConsole.js'
import BusControl from './BusControl.js'
import FooterControl from './FooterControl.js'
import GstdControl from './GstdControl.js'

var gstcG = new GstdClient("http://" + sessionStorage.address, sessionStorage.port);
if (document.getElementById("video_player")) {
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
        bus_enable: false,
        timeout: -1
    }
}

export default {
    name: 'video_player_app',
    components: {
        NavControl,
        VideoPlayer,
        SelectInput,
        ElementsControl,
        ChangeProperty,
        BusConsole,
        BusControl,
        FooterControl,
        GstdControl
    },
    data() {
        return {
            gstc: this.$datas.gstc,
            checked: this.$datas.checked,
            selected_input: this.$datas.selected_input,
            bus_enable: this.$datas.bus_enable,
            appConfig: false
        }
    },
    template: `
        <div class="container mx-auto p-4">
          <nav-control name="GSTD HTTP Video Player" ></nav-control>
          <gstd-control></gstd-control>
          <select-input v-model="checked" :config="appConfig" :selected="selected_input"></select-input>
          <video-player :config="appConfig" :name="gstc" :enable="checked"></video-player>
          <bus-console :myprop="bus_enable"></bus-console>
          <footer-control></footer-control>
        </div>
      `,
};