export default {
    name: 'BusControl',
    components: {},
    data() {
        return {
            text: "",
            text_tmp: "",
            filter: '',
            timeout: -1
        }
    },
    methods: {
        bus_timeout: async function(event) {
            var res = await this.$datas.gstc.bus_timeout("p0", this.timeout);
            console.log(res);
        },
        bus_filter: async function(event) {
            var res = await this.$datas.gstc.bus_filter("p0", this.filter);
            console.log(res);
        },
        bus_read_local: async function(event) {

            while (true) {
                var res = await this.$datas.gstc.bus_read("p0");
                this.text_tmp = res.response;
                this.text += JSON.stringify(this.text_tmp, null, 4) + "\n";
            }
        }
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
                </b-input-group>

            </div>
        

    `,
};

Vue.component('console-control', {



})