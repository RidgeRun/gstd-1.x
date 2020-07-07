/*
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

export default {
    name: 'NavControl',
    components: {},
    data: function() {
        return {
            count: 0
        }
    },
    props: ['name'],
    template: `
    <b-navbar style="" type="light" variant="light">
        <b-navbar-brand href="#">{{name}}</b-navbar-brand>
        <b-navbar-toggle target="navbar-toggle-collapse">
        </b-navbar-toggle>
        <b-collapse id="navbar-toggle-collapse" is-nav>
            <b-navbar-nav class="ml-auto">
            <b-button size="sm" class="my-2 my-sm-0"  variant="light" href="https://www.ridgerun.com/" target="_blank"><img src="./resources/logo.png" alt="Ridgerun" ></b-button>
            </b-navbar-nav>
        </b-collapse>
    </b-navbar>
  `,
};
