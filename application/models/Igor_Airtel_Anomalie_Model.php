<?php
    class Igor_Airtel_Anomalie_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_anomalie_ci");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_anomalie_ci", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_anomalie_ci", $data);
            }
        }

        public function insert_or_update_vi($data) {
            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_anomalie_vi");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_anomalie_vi", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_anomalie_vi", $data);
            }
        }

        public function insert_or_update_co($data) {
            
            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_anomalie_co");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_anomalie_co", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_anomalie_co", $data);
            }
        }

        public function get_anomalie_ci() {
            $this->db->select("*");
            $this->db->from("igor_airtel_anomalie_ci");
            $this->db->where_in("etat", array("Non", "En cours"));
            $query= $this->db->get();

            return $query->result();
        }

        public function get_anomalie_co() {
            $this->db->select("*");
            $this->db->from("igor_airtel_anomalie_co");
            $this->db->where_in("etat", array("Non", "En cours"));
            $query= $this->db->get();
            return $query->result();
        }

        public function delete_ci($igor) {
            $this->db->where("REF_IGOR", $igor);
            $this->db->delete("igor_airtel_anomalie_co");
        }

        public function get_vi() {
            $this->db->select('*');
            $this->db->select_sum('MONTANT', 'total');
            $this->db->from('igor_airtel_anomalie_vi');
            // $this->db->where("etat", "Non");
            $this->db->group_by('REF_IGOR');
            $query = $this->db->get();
            return $query->result();
        }

        public function delete_vi($ref_igor) {
            $this->db->where("REF_IGOR", $ref_igor);
            $this->db->delete("igor_airtel_anomalie_vi");
        }

        public function update_anomalie_co($ref_igor, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('REF_IGOR', $ref_igor);
            $this->db->update('igor_airtel_anomalie_co');
        }

        public function update_anomalie_ci($ref_igor, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('REF_IGOR', $ref_igor);
            $this->db->update('igor_airtel_anomalie_ci');
        }

        public function get_ci() {
            $this->db->select("*");
            $this->db->from("igor_airtel_anomalie_ci");
            return $this->db->get();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("igor_airtel_anomalie_co");
        }
        
    }

?>