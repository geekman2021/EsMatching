<?php 
    class Orange_Normal_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {

            $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            $exist= $this->db->get("boa_orange_normale_ci");
            if($exist->num_rows() == 0) {
                return $this->db->insert("boa_orange_normale_ci", $data);
            } else {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                return $this->db->update("boa_orange_normale_ci", $data);
            }
        }
        public function insert_or_update_co($data) {

            $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            $exist= $this->db->get("boa_orange_normale_co");
            if($exist->num_rows() == 0) {
                return $this->db->insert("boa_orange_normale_co", $data);
            } else {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                return $this->db->update("boa_orange_normale_co", $data);
            }
        }

        public function get_ci() {
            $this->db->select("*");
            $this->db->from("boa_orange_normale_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("boa_orange_normale_co");
            $query= $this->db->get();

            return $query->result();
        }

        

        
    }
?>