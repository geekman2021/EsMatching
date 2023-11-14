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
                $exist= $this->db->get("boa_telma_anomalie_co");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_anomalie_co", $data);
                } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                
                return $this->db->update("boa_telma_anomalie_co", $data);
            }
        }
        public function insert_or_update_nonAu($data) {

            $this->db->where("ref_igor", $data["ref_igor"]);
                $exist= $this->db->get("boa_telma_nonau");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("boa_telma_nonau", $data);
                } else {
                $this->db->where("ref_igor", $data["ref_igor"]);
                
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

        public function update_dat($ref_igor, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('REF_IGOR', $ref_igor);
            $this->db->update('igor_airtel_anomalie_co');
        }

        public function get_ci() {
            $this->db->select('boa_telma_anomalie_ci.*');
            $this->db->from('boa_telma_anomalie_ci');
            $this->db->join('boa_telma_nonau', 'boa_telma_anomalie_ci.REF_IGOR = boa_telma_nonau.ref_igor', 'left');
            $this->db->where('boa_telma_nonau.ref_igor IS NULL');
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