<?php
    class Igor_Airtel_Normal_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_normale_ci");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_normale_ci", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_normale_ci", $data);
            }
        }

        public function insert_or_update_co($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_normale_co");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_normale_co", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_normale_co", $data);
            }
        }

        
        
    }

?>