<?php
   defined('BASEPATH') OR exit('No direct script access allowed');
    
    class Igor_Airtel extends CI_Controller {
        public function __construct(){
            parent::__construct();
            session_start();
            $this->load->model("igor_airtel_anomalie_model");


        }

        public function edit_anomalie_CO() {
            $ref_igor= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->igor_airtel_anomalie_model->update_anomalie_CO($ref_igor, $etat, $date_regul);
        }

        public function edit_anomalie_CI() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");

            $this->igor_airtel_anomalie_model->update_anomalie_CI($id, $etat, $date_regul);
        }

        public function edit_anomalie_Vi() {

            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");

            $this->igor_airtel_model->igor_airtel_anomalie($id, $etat, $date_regul);

        }

    }


?>