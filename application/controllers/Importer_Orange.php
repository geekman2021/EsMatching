<?php

    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;

    class Importer_Orange extends CI_Controller {

        public $ambiguous= array();
        public $deallocation= array();
        public $airtel_anomalie_ci= array();
        public $airtel_anomalie_co= array();
        public $igor_anomalie_ci= array();
        public $igor_anomalie_co= array();


       
        public function __construct(){
            parent::__construct();
            
        }

        public function index() {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/orange-form");
        }

        public function importer() {
            
            $orange_read = $this->readData($_FILES["orange"]["name"], $_FILES["orange"]["tmp_name"]);
            $princ_read = $this->readData($_FILES["princ"]["name"], $_FILES["princ"]["tmp_name"]);
            $com_read= $this->readData($_FILES["com"]["name"], $_FILES["com"]["tmp_name"]);

            if (count($orange_read) > 1 ) {
                
                for($i=1; $i < count($orange_read); $i++) {

                    $debit= str_replace([" ", ","], "", $orange_read[$i][6]);
                    $credit= str_replace([" ", ","], "", $orange_read[$i][7]);
                    $cle= $orange_read[$i][5] .$debit .$credit ;
                    $super_distri= str_replace([" ", ".0"], "", $orange_read[$i][8]); 
                    $sous_distr= str_replace([" ", ".0"], "", $orange_read[$i][9]); 

                    // $debit = floatval($debit);
                    // $debit = intval($debit);
                    $orange[]= array(
                       "orange_num" => $orange_read[$i][0],
                       "orange_date" =>  $orange_read[$i][1],
                       "orange_heure" => $orange_read[$i][2],
                       "orange_ref" => $orange_read[$i][3],
                       "orange_service" => $orange_read[$i][4],
                       "orange_num_compte" => $orange_read[$i][5],
                       "orange_debit" => $debit,
                       "orange_credit" => $credit,
                       "orange_super_distri" => $super_distri,
                       "orange_sous_distri" => $sous_distr,
                       "cle" => $cle
                    );
                }
            }

            if (count($princ_read) > 1 ) {
                
                for($i=1; $i < count($princ_read); $i++) {

                    $montant = strval($princ_read[$i][4]);
                    $montant = str_replace(".00", "" , $montant);
                    $princ[]= array(
                       "princ_compte" => $princ_read[$i][0],
                       "princ_date_oper" =>  $princ_read[$i][1],
                       "princ_date_val" => $princ_read[$i][2],
                       "princ_devise" => $princ_read[$i][3],
                       "princ_montant" => $montant,
                       "princ_libelle" => $princ_read[$i][5],
                       "princ_oper" => $princ_read[$i][6],
                       "princ_expl" => $princ_read[$i][7],
                       "princ_ref_igor" => $princ_read[$i][8],
                       "princ_ref_rel" => $princ_read[$i][9],
                    );
                }
            }

            if (count($com_read) > 1 ) {
                
                for($i=1; $i < count($com_read); $i++) {

                    $codeAgence= $com_read[$i][9];
                    $codeAgence= explode("-", $codeAgence);
                    $comm[]= array(
                       "comm_compte" => $com_read[$i][0],
                       "comm_date_oper" =>  $com_read[$i][1],
                       "comm_date_val" => $com_read[$i][2],
                       "comm_devise" => $com_read[$i][3],
                       "comm_montant" => $com_read[$i][4],
                       "comm_libelle" => $com_read[$i][5],
                       "comm_oper" => $com_read[$i][6],
                       "comm_expl" => $com_read[$i][7],
                       "comm_ref_igor" => $com_read[$i][8],
                       "comm_ref_rel" => $com_read[$i][9],
                       "comm_code_agence" => $codeAgence[0]
                    );
                }
            }

            // echo "<pre>";
            //     print_r($princ);
            // echo "</pre>";

// --------------------------------------------------------------------------- PRINCIPALE -----------------------------------------------------------------


            $princ= $this->supprimerEspace($princ);
            $principalCI= $this->filtrerPrincCI($princ);
            $principalCI= $this->trierPrincParRefIgor($principalCI);

// --------------------------------------------------------------------------- COMMISSION -----------------------------------------------------------------
            $comm= $this->supprimerEspace($comm);
            $commCI= $this->filtrerCommCI($comm);
            $commCI= $this->trierCommParRefIgor($commCI);

            $mergedComEtPrinc = $this->mergePrincEtCommi($principalCI, $commCI);
            $mergedComEtPrinc = $this->trierParCleCroissante($mergedComEtPrinc);


// ---------------------------------------------------------------------------- ORANGE -------------------------------------------------------------------------------
            $orange= $this->supprimerEspace($orange);
            $ind= $this->filtrerInd($orange);
            $orangeCI= $this->filtrerOrangeCI($orange);
            $orangeCI= $this->trierParCleCroissante($orangeCI);
            $orangeCO= $this->filtrerOrangeCO($orange);
            $orangeCO = $this->trierParCleCroissante($orangeCO);

            $this->comparerBoaEtOrange($mergedComEtPrinc, $orangeCI);

            // echo "<pre>";
            //     print_r($orangeCI);
            // echo "</pre>";
            // echo "<pre>";
            //     print_r($mergedComEtPrinc);
            // echo "</pre>";
        }

        public function filtrerOrangeCI($data) {
            $resultat= array();
            foreach ($data as $item) {
                if($item["orange_service"] ==="Cashin") {
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerOrangeCO($data) {
            $resultat= array();
            foreach ($data as $item) {
                if($item["orange_service"] ==="CashOut") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }


        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension =="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

        }


        public function filtrerInd($data) {
            $resultat= array();

            foreach($data as $item) {
                if ($item["orange_num_compte"] ==="IND01") {
                    $resultat[] = $item;
                }
            }
            return $resultat;
        }

        public function supprimerEspace($data) {
            $resultat= array();
            foreach($data as $key=> $valeur) {
                $resultat[$key] = str_replace(" ", "", $valeur);
            }

            return $resultat;
        }

        public function supprimerEspaceOrange($data) {
            foreach ($data as &$item) {
                foreach ($item as $key => &$value) {
                    $value = preg_replace('/\s+/', '', $value);
                }
                $item['orange_debit'] = preg_replace('/\s+/', '', $item['orange_debit']);
            }
            return $data;
        }


        public function filtrerPrincCI($data) {
            $resultat= array();

            foreach($data as $item) {
                if (preg_match('/tel:(\d+)/', $item["princ_libelle"], $matches)) {
                    $numeroTelephone = $matches[1];
                    // echo "Numéro de téléphone : " . $numeroTelephone;
                } else {
                    // echo "Aucun numéro de téléphone trouvé.";
                }
                if($item["princ_oper"] ==="CASHI" && $item["princ_expl"] ==="AU") {
                    $cle = $numeroTelephone .$item["princ_montant"];
                    $item["cle"] = $cle;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerCommCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["comm_oper"] ==="CASHI" && $item["comm_expl"] ==="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }


        public function mergePrincEtCommi($principal, $commission) {
            $normalePrinc= array();
            $normaleComm= array();
            $mergedComEtPrinc= array();

            foreach($principal as $itemPrinc) {
                foreach($commission as $itemComm) {
                    if($itemPrinc["princ_ref_igor"] === $itemComm["comm_ref_igor"]) {
                        $normalePrinc[] = $itemPrinc;
                    }
                }
            }
            foreach($commission as $itemComm) {
                foreach($principal as $itemPrinc) {
                    if($itemPrinc["princ_ref_igor"] === $itemComm["comm_ref_igor"]) {
                        $normaleComm[] = $itemComm;
                    }
                }
            }

            $ligne= count($normalePrinc) > count($normaleComm) ? $normalePrinc : $normaleComm;

            for ($i =0; $i < count($normalePrinc) ; $i++) {
                $mergedComEtPrinc[] = array_merge($normalePrinc[$i], $normaleComm[$i]);
            }

            return $mergedComEtPrinc;
        }

        public function trierCommParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['comm_ref_igor'], $b['comm_ref_igor']);
            });

            return $data;
        }

        public function trierPrincParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['princ_ref_igor'], $b['princ_ref_igor']);
            });

            return $data;
        }

        public function trierParCleCroissante($data){
            usort($data, function ($a, $b) {
                $cleA = intval($a['cle']);
                $cleB = intval($b['cle']);

                if ($cleA == $cleB) {
                    return 0;
                }
                return ($cleA < $cleB) ? -1 : 1;
            });

            return $data;
        }

        public function comparerBoaEtOrange($boa, $orange) {
            $anomalieBoa = array();
            $anomalieOrange = array();
            $normaleOrange = array();
            $normaleBoa = array();
            $boaCopy = $boa;
            $orangeCopy = $orange;
        
            $ligne = min(count($boa), count($orange));
            $isFinished = false;
            $i = 0;
        
            while ($i < $ligne && !$isFinished) {
                if ($boaCopy[$i]["cle"] === $orangeCopy[$i]["cle"]) {
                    $normaleOrange[] = $orangeCopy[$i];
                    $normaleBoa[] = $boaCopy[$i];
                    $isFinished = true;
                    $i++;
                } elseif ($boaCopy[$i]["cle"] < $orangeCopy["cle"]) {
                    $anomalieBoa[] = $boaCopy[$i];
                    array_splice($boaCopy, $i, 1);
                    $boaCopy = array_values($boaCopy);
                } else {
                    $anomalieOrange[] = $orangeCopy;
                    array_splice($orange, $i, 1);
                    $orange = array_values($orange);
                }
            }

            
            echo "<pre>";
            print_r($normaleOrange);
            echo "</pre>";
        }
        
}

?>