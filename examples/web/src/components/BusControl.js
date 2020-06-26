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