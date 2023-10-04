<div class=" container mt-5">
    <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
            <a class="nav-link" data-toggle="tab" href="#tabBoa">BOA</a>
        </li>
        <li class="nav-item col-sm-6">
            <a class="nav-link" data-toggle="tab" href="#tabOrange">Orange</a>
        </li>
    </ul>
</div>


<div class="tab-content">
    <!-- BOA -->
    <div id="tabBoa" class="container mt-5 tab-pane "><br>  
        <div class="row">
        <div class="col-sm-6">
            <label for="dateDebut">Date Debut</label>
            <input type="text" class="form-control" id="minBoa" name="minBoa">
            </div>
            <div class="col-sm-6">
            <label for="dateDebut">Date Fin</label>
            <input type="text" class="form-control" id="maxBoa" name="maxBoa">
            </div> 
        </div> 

        <table class="table table-bordered" id="tableBoa">
            <thead style="text-align: center ;">
                <tr>
                <th>Compte</th>
                <th>Date_Oper</th>
                <th>Date_Val </th>
                <th>Devise</th>
                <th>Montant</th>
                <th>Libelle</th>
                <th>Operation</th>
                <th>Expl</th>
                <th>ReferenceIgor</th>
                <th>Action</th>
                </tr>
            </thead>
            <tbody>
            <tr>
                <td></td>
                <td></td>
                <td> </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td><a href="#" class="btn btn-info" data-toggle="modal" data-target="#myModalBoa" ></a></td>
            </tr>
            </tbody>
        </table>
    </div>

    
    <!-- ORANGE -->
    <div id="tabOrange" class="container mt-5 tab-pane ">
        <div class="row">
            <div class="col-sm-6">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="minOrange" name="minOrange">
            </div>
            <div class="col-sm-6">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="maxOrange" name="maxOrange">
            </div> 
        </div>
        <table id="tableOrange" class="table table-bordered">
            <thead>
                <tr>
                    <th>Date</th>
                    <th>Heure </th>
                    <th>Reférence</th>
                    <th>Serice</th>
                    <th>Compte</th>
                    <th>Débit</th>
                    <th>Crédit</th>
                    <th>Clé</th>
                    <th>Action</th>
                </tr>
            </thead>
            <tbody>
            <?php //foreach($ci as $item)  { ?>
            <tr> 
                <td></td>
                <td></td>
                <td> </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td><a href="#" data-toggle="modal" data-target="#myModalOrange<?php //echo $item->id; ?>">Afficher Infos</a></td>
            </tr>
            <?php //} ?>
            </tbody>
        </table>
    </div>
</div>

<script>
  let minDateBoa, maxDateBoa;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let minBoa = minDateBoa.val();
        let max = maxDateBoa.val();
        let date = new Date(data[1]);
    
        if (
            (minBoa === null && max === null) ||
            (minBoa === null && date <= max) ||
            (minBoa <= date && max === null) ||
            (minBoa <= date && date <= max)
        ) {
            return true;
        }
        return false;
    });
    
    // Create date inputs
    minDateBoa = new DateTime('#minBoa', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDateBoa = new DateTime('#maxBoa', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    // DataTables initialisation
    let tableBoa = new DataTable('#tableBoa', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });
    document.querySelectorAll('#minBoa, #maxBoa').forEach((el) => {
        el.addEventListener('change', () => tableBoa.draw());
    });
</script>

<script>
  let minDateOrange, maxDateOrange;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let minOrange = minDateOrange.val();
        let maxOrange = maxDateOrange.val();
        let date = new Date(data[1]);
    
        if (
            (minOrange === null && maxOrange === null) ||
            (minOrange === null && date <= maxOrange) ||
            (minOrange <= date && maxOrange === null) ||
            (minOrange <= date && date <= maxOrange)
        ) {
            return true;
        }
        return false;
    });
    
    // Create date inputs
    minDateOrange = new DateTime('#minOrange', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDateOrange = new DateTime('#maxOrange', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    // DataTables initialisation
    let tableOrange = new DataTable('#tableOrange', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });
    document.querySelectorAll('#minOrange, #maxOrange').forEach((el) => {
        el.addEventListener('change', () => tableOrange.draw());
    });
</script>



