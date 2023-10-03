<?php 
    class Boa_Telma_Anomalie_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_dat($data) {

                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                $exist= $this->db->get("boa_telma_anomalie_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_anomalie_ci", $data);
                } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                
                return $this->db->update("boa_telma_anomalie_ci", $data);
            }
        }

        public function insert_or_update_cat($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                $exist= $this->db->get("boa_telma_anomalie_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_anomalie_ci", $data);
                } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                
                return $this->db->update("boa_telma_anomalie_ci", $data);
            }
        }
        public function insert_or_update_nonAu($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                $exist= $this->db->get("boa_telma_nonau");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_nonau", $data);
                } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                
                return $this->db->update("boa_telma_nonau", $data);
            }
        }
        public function insert_or_update_vi($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                $exist= $this->db->get("boa_telma_anomalie_vi");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_anomalie_vi", $data);
                } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                
                return $this->db->update("boa_telma_anomalie_vi", $data);
            }
        }

        public function get_ci() {
            $this->db->select("*");
            $this->db->from("boa_telma_anomalie_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("boa_telma_anomalie_co");
            $query= $this->db->get();

            return $query->result();
        }
        public function get_vi() {
            $this->db->select("*");
            $this->db->from("boa_telma_anomalie_vi");
            $query= $this->db->get();

            return $query->result();
        }

    }
?>