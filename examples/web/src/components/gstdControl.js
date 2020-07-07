/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

export default {
    name: 'GstdControl',
    components: {},
    data: function() {
        return {
            count: 0,
            address: this.$datas.defaultAddress,
            port: this.$datas.defaultPort
        }
    },
    methods: {
        connect() {
            this.$datas.gstc = new GstdClient("http://" + this.address, this.port);
        }
    },
    props: ['name'],
    template: `
    <div>
        <div style="display:ruby;padding-right: 20px;height: 40px;">
            <b-form-group label-size="md" label-cols-sm="3" label="Address" label-align-lg="right" label-for="nested-street">
                <b-form-input  v-model="address" id="nested-street" size="md"></b-form-input>
            </b-form-group>
            <b-form-group label-size="md" label-cols-sm="3" label="Port" label-align-lg="right" label-for="nested-street" style="padding-right: 25px">
                <b-form-input  v-model="port" id="nested-street" size="md"></b-form-input>
            </b-form-group>
            <b-button  v-on:click="connect()">
                Configure
            </b-button>
        </div>
    </div>
  `,
};
