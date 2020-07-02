export default {
    name: 'BusControl',
    components: {},
    data() {
        return {
            text: "",
            filter: ''
        }
    },
    methods: {
        bus_filter: async function(event) {
            try {
                await this.$datas.gstc.bus_filter("p0", this.filter);
            } catch (error) {
                this.$root.$emit("console_write", "Filter", error);
                return;
            }
        },
        send_eos: async function(event) {
            try {
                await this.$datas.gstc.event_eos("p0");
            } catch (error) {
                this.$root.$emit("console_write", "EOS", error);
                return;
            }
        },
        flush_start: async function(event) {
            try {
                await this.$datas.gstc.event_flush_start("p0");
            } catch (error) {
                this.$root.$emit("console_write", "Flush", error);
                return;
            }
        },
        flush_stop: async function(event) {
            try {
                await this.$datas.gstc.event_flush_stop("p0");
            } catch (error) {
                this.$root.$emit("console_write", "Flush", error);
                return;
            }
        },
    },
    props: ['name', 'enable'],
    template: `
            <div>

                <b-input-group style="width: 400px;">
                    <b-input-group-prepend>
                        <h5 style="display: ruby;padding-top: 8px;padding-right: 10px;">Filters: </h5>
                </b-input-group-prepend>
                    <b-form-input v-model="filter" type="text"></b-form-input>
                        <b-input-group-append>
                        <b-button v-on:click="bus_filter()" variant="outline-secondary">Apply</b-button>
                    </b-input-group-append>
                
                </b-input-group label="hola">
                
                <div class="btn-group" role="group" style="display: block ruby;padding-bottom: 8px;padding-top: 8px;" aria-label="Basic example">
                    <h3>Send EOS: </h3>
                    <b-button v-on:click="send_eos()" variant="outline-secondary">Send</b-button>
                </div>
                <div class="btn-group" role="group" style="display: block ruby;padding-bottom: 8px;" aria-label="Basic example">
                <h3>Flush start: </h3>
                    <b-button v-on:click="flush_start()" variant="outline-secondary">Send</b-button>
                </div>
                <div class="btn-group" role="group" style="display: block ruby;padding-bottom: 8px;" aria-label="Basic example">
                    <h3>Flush stop: </h3>
                    <b-button v-on:click="flush_stop()" variant="outline-secondary">Send</b-button>
                </div>
            </div>
    `,
};
Vue.component('console-control', {
})
