<?php

    defined('BASEPATH') OR exit('No direct script access allowed');
    // require "vendor/autoload.php";
    

    class Orange extends CI_Controller {
        public function __construct(){
            parent::__construct();
        }
        public function index() {
            
            $this->load->view("templates/sidebar");
            $this->load->view("pages/operateur/orange-list");
        }
        public function edit_anomalie_CO() {

            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->airtel_anomalie_model->update_anomalie_co($id, $etat, $date_regul);

        }

        public function edit_anomalie_CI() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->airtel_anomalie_model->update_anomalie_CI($id, $etat, $date_regul);

        }

        public function edit_anomalie_deallo() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");

            $this->airtel_anomalie_model->update_anomalie_deallo($id, $etat, $date_regul);
    
        }
    }


    ?>



