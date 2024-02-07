<?php
    class User_Model extends CI_Model{
        public function __construct(){
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_user($data) {
            return $this->db->insert("utilisateurs", $data);
        }

        public function get_users() {
            $this->db->select('utilisateurs.*, profils.*');
            $this->db->from('utilisateurs');
            $this->db->join('profils', 'utilisateurs.code_profil = profils.code_profile');
            $query = $this->db->get();
            return $query->result();  
        }

        public function get_userBynumMat($matricule) {
            return $this->db->get_where("utilisateurs", array("matricule" =>$matricule))->row();
        }
        
        public function delete_user($id) {
            $this->db->where("id", $id);
            $this->db->delete("utilisateurs");
        }

        public function update_user($id, $data) {
            $this->db->where("id_utilisateur", $id);
            return $this->db->update("utilisateurs", $data);
        }

        public function find_user($id) {
            return $this->db->get_where("utilisateurs", array("id" =>$id))->row();
        }
    }
?>