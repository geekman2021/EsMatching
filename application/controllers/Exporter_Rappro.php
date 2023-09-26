<?php

    class Exporter_Rappro extends CI_Controller {
        public function __construct(){
            parent::__construct();
            $this->load->model("igor_airtel_anomalie_model");
            $this->load->model("historique_airtel_model");

        }

    }


?>