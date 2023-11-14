<?php
    class Regul_Igor_Airtel_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_vi($data) {
            $this->db->insert("regul_vi", $data);
            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $this->db->delete("igor_airtel_anomalie_vi");
        }

        public function insert_igor_airtel($data) {
            return $this->db->insert("regul_deallo", $data);
        }

        public function update_anomalie_ci($data) {
            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            return $this->db->update("igor_airtel_anomalie_ci", $data);
        }
    }
?>