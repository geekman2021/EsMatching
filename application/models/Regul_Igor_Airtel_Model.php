<?php
    class Regul_Igor_Airtel_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_vi($data) {
            return $this->db->insert("regul_vi", $data);
        }

        public function insert_igor_airtel($data) {
            return $this->db->insert("regul_deallo", $data);
        }
    }
?>