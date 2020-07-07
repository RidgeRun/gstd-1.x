/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

import NavControl from './navControl.js';
import VideoPlayer from './videoPlayer.js';
import SelectInput from './selectInput.js';
import ElementsControl from './elementsControl.js';
import ChangeProperty from './changeProperty.js';
import BusConsole from './busConsole.js'
import BusControl from './busControl.js'
import FooterControl from './footerControl.js'
import GstdControl from './gstdControl.js'

var gstc = new GstdClient("http://" + sessionStorage.address, sessionStorage.port);

if (document.getElementById("videoPlayer")) {
    Vue.prototype.$datas = {
        file: '',
        videoElement: null,
        paused: false,
        gstc: gstc,
        pipeName: "jsgstcPlayer",
        defaultAddress: "127.0.0.1",
        defaultPort: "5000",
        componentKey: 0,
        selectedElement: null,
        elements: [],
        selectedProperties: null,
        properties: [],
        speed: 1,
        direction: 1,
        checked: true,
        selectedInput: "File",
        busEnable: false,
        timeout: -1
    }
}

export default {
    name: 'videoPlayerApp',
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
            selectedInput: this.$datas.selectedInput,
            busEnable: this.$datas.busEnable,
            appConfig: false
        }
    },
    template: `
        <div class="container mx-auto p-4">
          <nav-control name="Gstd HTTP Video Player" ></nav-control>
          <gstd-control></gstd-control>
          <select-input v-model="checked" :config="appConfig" :selected="selectedInput"></select-input>
          <video-player :config="appConfig" :name="gstc" :enable="checked"></video-player>
          <bus-console :myprop="busEnable"></bus-console>
          <footer-control></footer-control>
        </div>
      `,
};
