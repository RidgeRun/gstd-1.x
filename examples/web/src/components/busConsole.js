/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

 export default {
    name: 'BusConsole',
    components: {},
    data() {
        return {
            text: "",
            textTmp: "",
            myToggle: false,
        }
    },
    methods: {
        bus_timeout: async function(event) {
            try {
                var res = await this.$datas.gstc.bus_timeout(this.$datas.pipeName, this.$datas.timeout);
            } catch (error) {
                this.$root.$emit("bus_timeout", "BusTimeout", error);
            }
        },
        bus_filter: async function(event) {
            try {
                var res = await this.$datas.gstc.bus_filter(this.$datas.pipeName, "error+warning+info");
            } catch (error) {
                this.$root.$emit("bus_filter", "BusFilter", error);
            }
        },
        bus_read_local: async function(event) {
            while (true) {
                try{
                    var res = await this.$datas.gstc.bus_read(this.$datas.pipeName);
                    if (res.response != null) {
                        this.textTmp = res.response;
                        this.text += JSON.stringify(this.textTmp, null, 4) + "\n";
                    }
                } catch (error) {
                    // If the pipe is deleted break
                    this.$root.$emit("bus_read", "BusRead");
                    break;
                }
            }
        },
        console_write: function(msg) {
            this.text += msg;
        }
    },

    updated() {

        var elem = this.$refs.messageDisplay.$refs.input;
        elem.scrollTop = elem.scrollTopMax;
    },

    props: ['name', 'enable'],
    mounted: function() {
        if (this.$datas.checked) {
            this.$root.$on('busevent', (text) => {
                this.bus_timeout();
                this.bus_filter();
                setTimeout(() => {
                    this.bus_read_local();
                }, 150);
            })
        }
        this.$root.$on("console_write", (func, msg) => {

            return this.console_write(func + ": " + msg + "\n");
        });
    },
    template: `
    <div>
        <h3 style="height: 50px;padding-top: 10px;">
            <b-badge variant="light">Console Log</b-badge>
        </h3>
        <div style="border:1px solid black;">
            <b-form-textarea ref='messageDisplay' placeholder="Pipeline log"  rows="5" max-rows="5" id="textarea" plaintext :value="text"></b-form-textarea>
        </div>
        <div>
            <!---<b-button-group>
                <b-button v-on:click="bus_filter()">Filter bus</b-button>
                <b-button v-on:click="bus_read()">Read bus</b-button>
            </b-button-group>-->
        </div>
    </div>
`,
};
