/*
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
            text_tmp: "",
            myToggle: false,
        }
    },
    methods: {
        bus_timeout: async function(event) {
            console.log(this.$datas.file);
            var res = await this.$datas.gstc.bus_timeout("p0", this.$datas.timeout);
            console.log(res);
        },
        bus_filter: async function(event) {
            var res = await this.$datas.gstc.bus_filter("p0", "error+warning+info");
            console.log(res);
        },
        bus_read_local: async function(event) {
            while (true) {
                var res = await this.$datas.gstc.bus_read("p0");
                if (res.response != null) {
                    this.text_tmp = res.response;
                    this.text += JSON.stringify(this.text_tmp, null, 4) + "\n";
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
