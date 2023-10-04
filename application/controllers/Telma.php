<?php

    defined('BASEPATH') OR exit('No direct script access allowed');
    // require "vendor/autoload.php";
    

    class Telma extends CI_Controller {
        public function __construct(){
            parent::__construct();
            $this->load->model("telma_normal_model");
            $this->load->model("telma_anomalie_model");
            $this->load->model("boa_telma_anomalie_model");

        }
        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/operateur/telma-list", $this->get_data());
        }

        public function get_data() {

            $data["normal_ci"] = $this->load->telma_normal_model->get_ci();
            $data["normal_co"] = $this->load->telma_normal_model->get_co();
            $data["vi"] = $this->load->boa_telma_anomalie_model->get_vi();
            $data["admin"] = $this->telma_anomalie_model->get_admin();
            $data["dat"] = $this->boa_telma_anomalie_model->get_ci();
            $data["cat"] = $this->boa_telma_anomalie_model->get_co();

            return $data;

        }
        
    }


    ?>



