/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

export default {
    name: 'ElementsControl',
    components: {},
    data() {
        return {
            selected: null,
            selectedElement: null,
            properties: [{ value: null, text: "Property" }],
            elements: [{ value: null, text: "Elements" }],
        }
    },
    methods: {
        getElements: async function() {
            this.elements = [];
            var res = await this.$datas.gstc.list_elements(this.$datas.pipeName);
            for (var i = 0; i < res.response.nodes.length; i++) {
                this.elements.push({ value: res.response.nodes[i].name, text: res.response.nodes[i].name });
            }
            this.$datas.elements = this.elements;

        },
        getProperties: async function() {
            this.properties = [];
            var res = await this.$datas.gstc.list_properties(this.$datas.pipeName, String(this.$datas.selectedElement));
            for (var i = 0; i < res.response.nodes.length; i++) {
                this.properties.push({ value: res.response.nodes[i].name, text: res.response.nodes[i].name });
            }
            this.$datas.properties = this.properties;

        },
        getSelectedItem: function() {
            this.$datas.selectedElement = this.selected;
            this.getProperties();
        },
        getSelectedItem_property: function() {
            this.$datas.selectedProperties = this.selectedElement;
        }
    },
    mounted: function() {
        this.$root.$on('myEvent', (text) => {
            this.getElements();
        })
    },
    props: ['name'],
    template: `
    <div style="display:;">
        <label>Element selection</label>
        <b-form-select v-on:change="getSelectedItem" style="display:flex;width: auto;" v-model="selected" :options="elements"></b-form-select>
        <label style="padding-top: 10px;">Property selection</label>
        <b-form-select v-on:change="getSelectedItem_property" style="display:flex;width: auto;" v-model="selectedElement" :options="properties"></b-form-select>
    </div>
  `,
};
