<?php 
    class Telma_Normal_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {

                $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("boa_telma_normale_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_normale_ci", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                
                return $this->db->update("boa_telma_normale_ci", $data);
            }
        }

        public function insert_or_update_co($data) {

            $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("boa_telma_normale_co");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_normale_co", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                
                return $this->db->update("boa_telma_normale_co", $data);
            }
        }

        public function get_ci() {
            $this->db->select("*");
            $this->db->from("boa_telma_normale_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("boa_telma_normale_co");
            $query= $this->db->get();

            return $query->result();
        }

    }
?>