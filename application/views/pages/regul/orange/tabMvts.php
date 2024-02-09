
<div id="tabMvts" class="container-fluid mt-1 tab-pane "><br>  
        <div class="row">
            <div class="col-sm-6 pl-4">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6 pr-4">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div> 
        <div class="container-fluid mt-5">
            <div class="card">
                <div class="card-header">
                    <b>Liste MVTS Regularisé</b>
                </div>
                <div class="card-body">
                    <table class="table table-bordered" id="tableMvts">
                <thead style="text-align: center ;">
                    <tr>
                        <th>Date</th>
                        <th>Heure</th>
                        <th>Réference</th>
                        <th>Service</th>
                        <th>Num_Compte</th>
                        <th>Débit</th>
                        <th>Montant</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody>
                </tbody>
            </table>
                </div>
            </div>
            
        </div>
    </div>
    <?php $this->load->view("pages/regul/orange/modal/modalMvts") ?>