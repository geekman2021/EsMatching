<?php
    class Regul_Airtel_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert($data) {
            return $this->db->insert("regul_airtel", $data);
        }

        public function insert_vi_deallo ($data) {
            return $this->db->insert("regul_vi_deallo", $data);
        }

        public function insert_igor_airtel($data) {
            return $this->db->insert("regul_igor_airtel", $data);
        }
    }
?>